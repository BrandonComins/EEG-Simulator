#include "plothelper.h"

#include "scaledialog.h"
#include "stylecurvediag.h"

#include <qboxlayout.h>
#include <qtimer.h>
#include <qwt_legend.h>
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_magnifier.h>
#include <qwt_scale_div.h>
#include <qwt_text.h>
#include <qwt_picker_machine.h>
#include <QFrame>
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>

Plot::PlotHelper::PlotHelper(QFrame *frame, QObject* parent)
    : QObject(parent)
    , m_paused(false)
    , m_auto_scale_x(false)
    , m_auto_scale_y(false)
    , m_view_mode(CUMULATIVE)
    , m_scale_diag(new ScaleDialog())
    , m_plot(new QwtPlot(frame))
    , m_picker(nullptr)
{
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_plot);

    auto* canvas = qobject_cast<QwtPlotCanvas*>(m_plot->canvas());

    m_grid = new QwtPlotGrid();
    m_grid->enableX(true);
    m_grid->enableY(true);
    m_grid->setPen(Qt::black, 0.5);
    m_grid->setZ(-1.0);
    m_grid->attach(m_plot);

    m_zoomer = new QwtPlotZoomer(canvas);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::NoButton);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::NoButton);

    m_magnifier = new QwtPlotMagnifier(canvas);
    m_magnifier->setMouseButton(Qt::NoButton); // Scroll wheel only

    m_legend = new QwtPlotLegendItem();
    m_legend->attach(m_plot);
    m_legend->setVisible(false);

    canvas->setContextMenuPolicy(Qt::CustomContextMenu);

    MultiSeriesTracker *tracker = new MultiSeriesTracker(canvas);
    tracker->setRubberBandPen(QPen(Qt::yellow, 1, Qt::DotLine));
    tracker->setTrackerPen(QPen(Qt::yellow));

    QObject::connect(canvas, &QWidget::customContextMenuRequested,
            this, &PlotHelper::show_context_menu, Qt::UniqueConnection);

    QFile file(QStringLiteral(":/ui/resources/modern.qss"));
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        frame->setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }

    QObject::connect(m_scale_diag, &ScaleDialog::auto_scale_clicked, this,
                     &PlotHelper::auto_scale, Qt::UniqueConnection);

    QObject::connect(m_scale_diag, &ScaleDialog::scale_clicked, this,
                     &PlotHelper::manual_scale, Qt::UniqueConnection);

}

Plot::PlotHelper::~PlotHelper() {
    delete m_scale_diag;
}

void Plot::PlotHelper::set_axis_titles(const std::string &x_title, const std::string &y_title) {
    m_plot->setAxisTitle(QwtPlot::xBottom, QString::fromStdString(x_title));
    m_plot->setAxisTitle(QwtPlot::yLeft, QString::fromStdString(y_title));

    m_plot->replot();
}

void Plot::PlotHelper::add_series(const std::string &id, CurveConfig config) {
    Series series;
    series.config = config;

    if (series.config.color == Qt::transparent) {
        series.config.color = default_palette.at(m_series.size() % default_palette.size());
    }
    if (series.config.z_order == 0.0) {
        series.config.z_order = static_cast<double>(m_series.size() + 1);
    }

    series.curve = new QwtPlotCurve(QString::fromStdString(id));
    series.curve->attach(m_plot);

    m_series.insert_or_assign(id, series);
    m_series_order.push_back(id);
    m_style_dialogs.insert_or_assign(id, new Plot::StyleCurveDiag(id, m_series.at(id).config));

    update_series_style(id, m_series.at(id).config);

    QObject::connect(m_style_dialogs.at(id), &StyleCurveDiag::settings_changed, this, [&] (const std::string &id){
        update_series_style(id, m_series.at(id).config);
        m_plot->replot();
    });
}

void Plot::PlotHelper::update_series_style(const std::string &id, const CurveConfig &config) {
    auto it = m_series.find(id);
    if (it != m_series.end()) {
        auto &series = it->second;
        auto *curve = series.curve;

        QPen pen(config.color, config.line_width, config.line_style);
        curve->setPen(pen);
        curve->setZ(config.z_order);
        curve->setRenderHint(QwtPlotItem::RenderAntialiased, config.use_antialiasing);
        curve->setStyle(config.curve_style);

        constexpr int pen_size = 1;
        constexpr int side = 8;

        curve->setSymbol(new QwtSymbol(config.symbol_type,
                                       QBrush(config.color),
                                       QPen(Qt::white, pen_size),
                                       QSize(side, side)));

        curve->setRawSamples(series.x_data.data(),
                             series.y_data.data(),
                             static_cast<int>(series.x_data.size()));
    }
}

void Plot::PlotHelper::add_point(const std::string &id, double x, double y) {
    auto it = m_series.find(id);

    if (it != m_series.end()) {
        auto& series = it->second;

        series.x_data.push_back(x);
        series.y_data.push_back(y + series.config.y_offset);

        if(!m_paused) {
            if (m_view_mode == PlotMode::ROLLING || m_view_mode == PlotMode::SWEEP) {
                m_plot->setAxisAutoScale(QwtPlot::xBottom, false);

                const QwtScaleDiv &scaleDiv = m_plot->axisScaleDiv(QwtPlot::xBottom);
                const double window_width = scaleDiv.upperBound() - scaleDiv.lowerBound();

                if (x > scaleDiv.upperBound()) {
                    if (m_view_mode == PlotMode::ROLLING) {
                        m_plot->setAxisScale(QwtPlot::xBottom, x - window_width, x);
                    }
                    else if (m_view_mode == PlotMode::SWEEP) {
                        double next_page_start = scaleDiv.upperBound();
                        m_plot->setAxisScale(QwtPlot::xBottom, next_page_start, next_page_start + window_width);
                    }
                }
            }
            else if (m_auto_scale_x) {
                m_plot->setAxisAutoScale(QwtPlot::xBottom, true);
            }

            series.curve->setSamples(series.x_data.data(),
                                     series.y_data.data(),
                                     static_cast<int>(series.x_data.size()));

            m_plot->replot();
        }
    } else {
        add_series(id);
        add_point(id, x, y);
    }
}

void Plot::PlotHelper::set_view_mode(PlotMode view_mode) {
    m_view_mode = view_mode;

    if (m_view_mode == ROLLING || m_view_mode == SWEEP) {
        m_plot->setAxisAutoScale(QwtPlot::xBottom, false);
    } else {
        m_plot->setAxisAutoScale(QwtPlot::xBottom, m_auto_scale_x);
    }
    m_plot->replot();
}

void Plot::PlotHelper::set_auto_scale_x(bool auto_scale) {
    m_auto_scale_x = auto_scale;

    if (m_view_mode == CUMULATIVE) {
        m_plot->setAxisAutoScale(QwtPlot::xBottom, auto_scale);
        m_plot->replot();
    }
}

void Plot::PlotHelper::set_auto_scale_y(bool auto_scale) {
    m_auto_scale_y = auto_scale;

    if (m_view_mode == CUMULATIVE) {
        m_plot->setAxisAutoScale(QwtPlot::yLeft, auto_scale);
        m_plot->replot();
    }
}

void Plot::PlotHelper::set_scale_x(double min, double max) {
    m_plot->setAxisScale(QwtPlot::xBottom, min, max);

    if (m_view_mode == PlotMode::ROLLING || m_view_mode == PlotMode::SWEEP) {
        m_plot->setAxisAutoScale(QwtPlot::xBottom, false);
    } else {
        m_plot->setAxisAutoScale(QwtPlot::xBottom, m_auto_scale_x);
    }

    m_plot->replot();
}

void Plot::PlotHelper::set_scale_y(double min, double max) {
    m_plot->setAxisScale(QwtPlot::yLeft, min, max);
    m_plot->replot();
}

auto Plot::PlotHelper::plot() const -> QwtPlot * {
    return m_plot;
}

void Plot::PlotHelper::manual_scale(double min_x, double max_x, double min_y, double max_y) {
    if (m_view_mode == PlotMode::ROLLING || m_view_mode == PlotMode::SWEEP) {
        m_plot->setAxisScale(QwtPlot::xBottom, min_x, max_x);
    } else {
        m_plot->setAxisScale(QwtPlot::xBottom, min_x, max_x);
    }

    m_plot->setAxisAutoScale(QwtPlot::yLeft, false);
    m_plot->setAxisScale(QwtPlot::yLeft, min_y, max_y);

    m_plot->replot();
}

void Plot::PlotHelper::set_x_axis_visible(bool visible) {
    m_plot->enableAxis(QwtPlot::xBottom, visible);
    m_plot->replot();
}

void Plot::PlotHelper::set_y_axis_visible(bool visible) {
    m_plot->enableAxis(QwtPlot::yLeft, visible);
    m_plot->replot();
}

void Plot::PlotHelper::set_x_axis_title_visible(bool visible) {
    m_plot->setAxisTitle(QwtPlot::xBottom, visible ? m_x_axis_title : QStringLiteral(""));
    m_plot->replot();
}

void Plot::PlotHelper::set_y_axis_title_visible(bool visible) {
    m_plot->setAxisTitle(QwtPlot::yLeft, visible ? m_y_axis_title : QStringLiteral(""));
    m_plot->replot();
}

void Plot::PlotHelper::save_file(Plot::FileType file_type) {
    QString default_dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString filter;
    QString default_ext;

    if (file_type == FileType::CSV) {
        filter = tr("CSV Files (*.csv);;Text Files (*.txt)");
        default_ext = ".csv";
    } else {
        filter = tr("PNG Image (*.png);;PDF Document (*.pdf)");
        default_ext = ".png";
    }

    QString path = QFileDialog::getSaveFileName(
        nullptr,
        tr("Export Data"),
        default_dir + "/eeg_export" + default_ext,
        filter
        );

    if (!path.isEmpty()) {
        if (file_type == FileType::CSV) {
            export_csv(path);
        } else {
            export_image(path);
        }
    }
}

void Plot::PlotHelper::export_csv(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);

    out << "Time";
    for (const auto& [id, series] : m_series) {
        out << "," << QString::fromStdString(id);
    }
    out << "\n";

    if (m_series.empty()) return;

    const auto& first_series = m_series.begin()->second;
    for (size_t i = 0; i < first_series.x_data.size(); ++i) {
        out << first_series.x_data[i];

        for (const auto& [id, series] : m_series) {
            if (i < series.y_data.size()) {
                out << "," << series.y_data[i];
            } else {
                out << ",0";
            }
        }
        out << "\n";
    }
    file.close();
}

void Plot::PlotHelper::export_image(const QString &path) {
    QPixmap pixmap(m_plot->size());
    m_plot->render(&pixmap);
    pixmap.save(path);
}

void Plot::PlotHelper::auto_scale() {
    m_plot->setAxisAutoScale(QwtPlot::xBottom, true);
    m_plot->setAxisAutoScale(QwtPlot::yLeft, true);
    m_plot->updateAxes();
    m_plot->replot();
}

void Plot::PlotHelper::set_legend_visible(bool visible) {
    m_legend->setVisible(visible);
}

void Plot::PlotHelper::set_axis_visible(bool x_axis_visible, bool y_axis_visible) {
    set_x_axis_visible(x_axis_visible);
    set_y_axis_visible(y_axis_visible);
}

void Plot::PlotHelper::set_axis_titles_visible(bool x_title_visible, bool y_title_visible) {
    set_x_axis_title_visible(x_title_visible);
    set_y_axis_title_visible(y_title_visible);
}

void Plot::PlotHelper::set_grid_visible(bool visible) {
    m_grid->setVisible(visible);
    m_plot->replot();
}

void Plot::PlotHelper::show_context_menu(const QPoint& pos) {
    QMenu menu;

    QMenu *scale_menu = menu.addMenu("Scale");

    auto *auto_action = scale_menu->addAction("Auto Scale All");
    QObject::connect(auto_action, &QAction::triggered, this, &PlotHelper::auto_scale);

    auto *manual_action = scale_menu->addAction("Manual Scale...");
    QObject::connect(manual_action, &QAction::triggered, [&] {
        if (m_scale_diag) {
            m_scale_diag->show();
        }
    });

    QMenu *plot_mode_menu = menu.addMenu("Plot Mode");

    auto* rolling_action = plot_mode_menu->addAction("Rolling");
    rolling_action->setCheckable(true);
    rolling_action->setChecked(m_view_mode == PlotMode::ROLLING);
    QObject::connect(rolling_action, &QAction::triggered, [&] {
        set_view_mode(PlotMode::ROLLING);
    });

    auto *sweep_action = plot_mode_menu->addAction("Sweep");
    sweep_action->setCheckable(true);
    sweep_action->setChecked(m_view_mode == PlotMode::SWEEP);
    QObject::connect(sweep_action, &QAction::triggered, [&] {
        set_view_mode(PlotMode::SWEEP);
    });

    auto *cumulative_action = plot_mode_menu->addAction("Cumulative");
    cumulative_action->setCheckable(true);
    cumulative_action->setChecked(m_view_mode == PlotMode::CUMULATIVE);
    QObject::connect(cumulative_action, &QAction::triggered, [&] {
        set_view_mode(PlotMode::CUMULATIVE);
    });

    menu.addSeparator();

    QMenu *plot_control_menu = menu.addMenu("Plot Control");

    auto *pause_action = plot_control_menu->addAction(m_paused
                                            ? QStringLiteral("Resume")
                                            : QStringLiteral("Pause"));
    QObject::connect(pause_action, &QAction::triggered, [&] {
        m_paused = !m_paused;
    });

    auto *clear_action = plot_control_menu->addAction("Clear Plot");
    QObject::connect(clear_action, &QAction::triggered, [&] {
        for (auto& [id, series] : m_series) {
            series.x_data.clear();
            series.y_data.clear();
            series.curve->setSamples(static_cast<double*>(nullptr),
                                     static_cast<double*>(nullptr), 0);
        }
        m_plot->replot();
    });

    QMenu *style_menu = menu.addMenu("Style");
    for(auto const &id : std::as_const(m_series_order)) {
        auto style_action = style_menu->addAction(QString::fromStdString(id));
        QObject::connect(style_action, &QAction::triggered, this, [&]{
            m_style_dialogs.at(id)->show();
        });
    }

    menu.addSeparator();

    QMenu *view_menu = menu.addMenu("View");

    auto *grid_action = view_menu->addAction("Show Grid");
    grid_action->setCheckable(true);
    grid_action->setChecked(m_grid->isVisible());
    QObject::connect(grid_action, &QAction::triggered, [&](bool checked) {
        set_grid_visible(checked);
    });

    auto *legend_action = view_menu->addAction("Show Legend");
    legend_action->setCheckable(true);

    legend_action->setChecked(m_legend->isVisible());

    QObject::connect(legend_action, &QAction::triggered, [&](bool checked) {{
        set_legend_visible(checked);
    }});

    view_menu->addSeparator();

    QMenu *titles_menu = view_menu->addMenu("Axis Titles");

    auto *x_title_act = titles_menu->addAction("Show X Title");
    x_title_act->setCheckable(true);
    x_title_act->setChecked(!m_plot->axisTitle(QwtPlot::xBottom).isEmpty());
    QObject::connect(x_title_act, &QAction::triggered, [this](bool checked) {
        set_x_axis_title_visible(checked);
    });

    auto *y_title_act = titles_menu->addAction("Show Y Title");
    y_title_act->setCheckable(true);
    y_title_act->setChecked(!m_plot->axisTitle(QwtPlot::yLeft).isEmpty());
    QObject::connect(y_title_act, &QAction::triggered, [this](bool checked) {
        set_y_axis_title_visible(checked);
    });

    QMenu *ticks_menu = view_menu->addMenu("Axis Ticks/Labels");
    auto *x_ticks_act = ticks_menu->addAction("Show X Ticks");
    x_ticks_act->setCheckable(true);
    x_ticks_act->setChecked(m_plot->axisEnabled(QwtPlot::xBottom));
    QObject::connect(x_ticks_act, &QAction::triggered, [this](bool checked) {
        set_x_axis_visible(checked);
    });

    auto *y_ticks_act = ticks_menu->addAction("Show Y Ticks");
    y_ticks_act->setCheckable(true);
    y_ticks_act->setChecked(m_plot->axisEnabled(QwtPlot::yLeft));
    QObject::connect(y_ticks_act, &QAction::triggered, [this](bool checked) {
        set_y_axis_visible(checked);
    });

    menu.addSeparator();

    QMenu *save_menu = menu.addMenu("Save");

    auto *export_action = save_menu->addAction("Export to CSV...");
    QObject::connect(export_action, &QAction::triggered, [&] {
        save_file(FileType::CSV);
    });

    auto *screenshot_action = save_menu->addAction("Save Screenshot...");
    QObject::connect(screenshot_action, &QAction::triggered, [&] {
        save_file(FileType::IMAGE);
    });

    menu.exec(m_plot->canvas()->mapToGlobal(pos));
}