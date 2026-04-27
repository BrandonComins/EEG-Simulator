#include "multiseriestracker.h"

#include "qwt_picker_machine.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_dict.h"
#include "qwt_plot_item.h"
#include "qwt_text.h"
#include <qwidget.h>

void Plot::MultiSeriesTracker::move(const QPoint &pos) {
    QPointF plotPos = invTransform(pos);
    double closestDist = std::numeric_limits<double>::max();
    QPointF snapPoint = plotPos;

    const QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);
    for (const QwtPlotItem* item : items) {
        const QwtPlotCurve* curve = static_cast<const QwtPlotCurve*>(item);

        double dist;
        int index = curve->closestPoint(plotPos, &dist);
        if (index != -1) {
            QPointF p = curve->sample(index);
            double dx = p.x() - plotPos.x();
            double dy = p.y() - plotPos.y();
            double currentDist = dx * dx + dy * dy;

            if (currentDist < closestDist) {
                closestDist = currentDist;
                snapPoint = p;
            }
        }
    }

    QwtPlotPicker::move(transform(snapPoint));
}

Plot::MultiSeriesTracker::MultiSeriesTracker(QWidget *canvas) :
    QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft,
                  QwtPlotPicker::CrossRubberBand,
                  QwtPicker::AlwaysOn, canvas)
{
    setStateMachine(new QwtPickerTrackerMachine());
}

QwtText Plot::MultiSeriesTracker::trackerTextF(const QPointF &pos) const {
    QString label;
    const QwtPlotItemList items = plot()->itemList(QwtPlotItem::Rtti_PlotCurve);

    label += QString("X: %1\n").arg(pos.x(), 0, 'f', 2);
    label += "------------------\n";

    for (const QwtPlotItem* item : items) {
        const QwtPlotCurve* curve = static_cast<const QwtPlotCurve*>(item);

        double dist;
        int index = curve->closestPoint(pos, &dist);

        if (index != -1) {
            QPointF p = curve->sample(index);
            label += QString("%1: %2\n")
                         .arg(curve->title().text())
                         .arg(p.y(), 0, 'f', 2);
        }
    }

    QwtText qwtText(label.trimmed());
    qwtText.setBackgroundBrush(QBrush(QColor(30, 30, 30, 180)));
    qwtText.setColor(Qt::white);

    return qwtText;
}
