#include "plothelper.h"
#include <qwt_plot.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_magnifier.h>
#include <QFrame>
#include <QMenu>

Plot::PlotHelper::PlotHelper(QFrame *frame, QObject* parent)
    : QObject(parent)
    , m_plot(new QwtPlot(frame))
{
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

    canvas->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(canvas, &QWidget::customContextMenuRequested,
            this, &PlotHelper::show_context_menu);
}

void Plot::PlotHelper::add_series(const std::string &id, Curve config) {
    if (m_curves.find(id) != m_curves.end()) return;

    if (config.color == Qt::transparent) {
        config.color = default_palette[m_curves.size() % default_palette.size()];
    }

    if (config.z_order == 0.0) {
        config.z_order = static_cast<double>(m_curves.size() + 1);
    }

    auto* curve = new QwtPlotCurve(QString::fromStdString(id));
    m_curves[id] = curve;

    update_series_style(id, config);
    curve->attach(m_plot);

    m_plot->replot();
}

void Plot::PlotHelper::update_series_style(const std::string &id, const Curve &config) {
    auto it = m_curves.find(id);
    if (it == m_curves.end()) return;

    auto* curve = it->second;

    QPen pen(config.color, config.line_width, config.line_style);
    curve->setPen(pen);
    curve->setZ(config.z_order);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, config.use_antialiasing);
    curve->setStyle(config.curve_style);

    if (config.symbol_type != QwtSymbol::NoSymbol) {
        curve->setSymbol(new QwtSymbol(config.symbol_type,
                                       QBrush(config.color),
                                       QPen(Qt::white, 1),
                                       QSize(8, 8)));
    } else {
        curve->setSymbol(nullptr);
    }
}

auto Plot::PlotHelper::plot() const -> QwtPlot * {
    return m_plot;
}

void Plot::PlotHelper::auto_scale() {
    m_plot->setAxisAutoScale(QwtPlot::xBottom, true);
    m_plot->setAxisAutoScale(QwtPlot::yLeft, true);
    m_plot->updateAxes();
    m_plot->replot();
}

void Plot::PlotHelper::show_context_menu(const QPoint& pos) {
    QMenu menu;
    auto* action = menu.addAction("Autoscale View");
    connect(action, &QAction::triggered, this, &PlotHelper::auto_scale);
    menu.exec(m_plot->canvas()->mapToGlobal(pos));
}