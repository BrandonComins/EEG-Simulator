#include "peakmanager.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_scale_div.h>
#include <qwt_symbol.h>

Plot::PeakManager::PeakManager(QwtPlot *plot)
    : QObject(plot)
    , m_plot(plot)
    , m_show_peaks(false)
    , m_show_mins(false) {}

void Plot::PeakManager::set_peaks_enabled(bool enabled) {
    m_show_peaks = enabled;
    refresh_peaks();
}

void Plot::PeakManager::set_mins_enabled(bool enabled) {
    m_show_mins = enabled;
    refresh_peaks();
}

auto Plot::PeakManager::peaks_enabled() const -> bool {
    return m_show_peaks;
}

auto Plot::PeakManager::mins_enabled() const -> bool {
    return m_show_mins;
}

auto Plot::PeakManager::enabled() const -> bool {
    return m_show_peaks || m_show_mins;
}

void Plot::PeakManager::clear_markers() {
    for (auto &marker : m_active_markers) {
        marker->detach();
        delete marker;
    }
    m_active_markers.clear();
}

void Plot::PeakManager::add_peak_marker(const QPointF &point, const QColor &color, bool is_peak) {
    QwtPlotMarker *marker = new QwtPlotMarker();

    QwtSymbol::Style symbol_style = is_peak ? QwtSymbol::Triangle : QwtSymbol::DTriangle;
    QColor symbol_color = is_peak ? color : color.darker(150);

    QwtSymbol *symbol =
        new QwtSymbol(symbol_style, QBrush(symbol_color), QPen(Qt::white, 1), QSize(8, 8));

    marker->setSymbol(symbol);
    marker->setValue(point);
    marker->attach(m_plot);
    m_active_markers.push_back(marker);
}

void Plot::PeakManager::refresh_peaks() {
    if (m_plot && (m_show_peaks || m_show_mins)) {
        clear_markers();

        const QwtScaleDiv scale_div = m_plot->axisScaleDiv(QwtPlot::xBottom);
        const double x_min = scale_div.lowerBound();
        const double x_max = scale_div.upperBound();

        const QwtPlotItemList items = m_plot->itemList(QwtPlotItem::Rtti_PlotCurve);

        for (const QwtPlotItem *item : items) {
            if (item->isVisible()) {
                const auto *curve = static_cast<const QwtPlotCurve *>(item);
                if (curve->dataSize() >= 3) {
                    for (size_t i = 1; i < curve->dataSize() - 1; ++i) {
                        QPointF p_prev = curve->sample(i - 1);
                        QPointF p_curr = curve->sample(i);
                        QPointF p_next = curve->sample(i + 1);

                        if (p_curr.x() >= x_min && p_curr.x() <= x_max) {
                            if (m_show_peaks && p_curr.y() > p_prev.y() &&
                                p_curr.y() > p_next.y()) {
                                if (p_curr.y() > 0.1) {
                                    add_peak_marker(p_curr, curve->pen().color(), true);
                                }
                            } else if (m_show_mins && p_curr.y() < p_prev.y() &&
                                       p_curr.y() < p_next.y()) {
                                if (p_curr.y() < -0.1) {
                                    add_peak_marker(p_curr, curve->pen().color(), false);
                                }
                            }
                        }
                    }
                }
            }
        }
        m_plot->replot();
    } else if (m_plot) {
        clear_markers();
        m_plot->replot();
    }
}
