#include "multiseriestracker.h"

#include "qwt_text.h"

#include <QCursor>
#include <QFont>
#include <QPen>
#include <qwt_picker_machine.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_dict.h>
#include <qwt_plot_item.h>
#include <qwt_scale_div.h>
#include <qwt_scale_map.h>

Plot::MultiSeriesTracker::MultiSeriesTracker(QWidget *canvas)
    : QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::VLineRubberBand,
                    QwtPicker::AlwaysOn, canvas) {

    setStateMachine(new QwtPickerTrackerMachine());
    setRubberBandPen(QPen(Qt::blue, 1, Qt::DashLine));
    setTrackerPen(QPen(Qt::blue));
}

void Plot::MultiSeriesTracker::move(const QPoint &pos) {
    if (plot() && plot()->canvas()) {
        const QwtScaleMap x_map = plot()->canvasMap(xAxis());
        const QwtScaleMap y_map = plot()->canvasMap(yAxis());

        double mouse_data_x = x_map.invTransform(pos.x());
        const QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);

        int target_y_pixel = -1;
        double min_x_dist = std::numeric_limits<double>::max();

        for (const QwtPlotItem *item : items) {
            if (item->isVisible()) {
                const auto *curve = static_cast<const QwtPlotCurve *>(item);
                double dist;
                int index = curve->closestPoint(QPointF(mouse_data_x, 0.0), &dist);

                if (index != -1) {
                    QPointF sample = curve->sample(index);
                    double x_diff = std::abs(sample.x() - mouse_data_x);

                    if (x_diff < min_x_dist) {
                        min_x_dist = x_diff;
                        target_y_pixel = qRound(y_map.transform(sample.y()));
                    }
                }
            }
        }

        if (target_y_pixel != -1) {
            QwtPlotPicker::move(QPoint(pos.x(), target_y_pixel));
        } else {
            QwtPlotPicker::move(pos);
        }
    }
}

QwtText Plot::MultiSeriesTracker::trackerTextF(const QPointF &pos) const {
    QwtText qwt_text_result;

    if (plot()) {
        QString label = " ";
        label += QString("X = %1").arg(pos.x(), 0, 'f', 3);
        label += " \n ---------------------- \n";

        const QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
        const int pixel_threshold = 15;
        const QwtScaleMap x_map = plot()->canvasMap(xAxis());
        int mouse_pixel_x = x_map.transform(pos.x());

        for (const QwtPlotItem *item : items) {
            if (item->isVisible()) {
                const auto *curve = static_cast<const QwtPlotCurve *>(item);
                int index = -1;
                double min_pixel_dist = pixel_threshold;
                QPointF found_sample;

                for (size_t i = 0; i < curve->dataSize(); ++i) {
                    QPointF sample = curve->sample(i);
                    int sample_pixel_x = x_map.transform(sample.x());
                    int diff = std::abs(sample_pixel_x - mouse_pixel_x);

                    if (diff < min_pixel_dist) {
                        min_pixel_dist = diff;
                        index = i;
                        found_sample = sample;
                    }
                }

                if (index != -1) {
                    label += QString(" %1: %2 \n")
                                 .arg(curve->title().text(), -12)
                                 .arg(found_sample.y(), 8, 'f', 2);
                } else {
                    label += QString(" %1: -- \n").arg(curve->title().text(), -12);
                }
            }
        }

        qwt_text_result.setText(label.trimmed());
        QFont font("Monospace");
        font.setStyleHint(QFont::TypeWriter);
        font.setPointSize(9);
        qwt_text_result.setFont(font);
        qwt_text_result.setBackgroundBrush(QBrush(QColor(25, 25, 25, 220)));
        qwt_text_result.setColor(Qt::cyan);
        qwt_text_result.setPaintAttribute(QwtText::PaintBackground, true);
    }

    return qwt_text_result;
}