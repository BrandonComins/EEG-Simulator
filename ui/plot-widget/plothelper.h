#ifndef PLOTHELPER_H
#define PLOTHELPER_H

#include "qwt_plot_curve.h"
#include <QObject>
#include <QVector>
#include <qcolor.h>
#include <qwt_symbol.h>

class QFrame;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotMagnifier;
class QwtPlotZoomer;

namespace Plot {


const std::vector<QColor> default_palette = {
    QColor(0x007AFF), // Blue
    QColor(0xFF3B30), // Red
    QColor(0x34C759), // Green
    QColor(0xAF52DE), // Purple
    QColor(0xFF9500), // Orange
    QColor(0x5856D6), // Indigo
    QColor(0xFFCC00), // Yellow
    QColor(0x5AC8FA)  // Light Blue
};

struct Curve {
    QColor color = Qt::transparent; // Triggers auto-palette
    float line_width = 1.5f;
    Qt::PenStyle line_style = Qt::SolidLine;
    double vertical_offset = 0.0;
    double z_order = 0.0;
    bool use_antialiasing = true;

    QwtPlotCurve::CurveStyle curve_style = QwtPlotCurve::Lines;
    QwtSymbol::Style symbol_type = QwtSymbol::NoSymbol;
};
class PlotHelper : public QObject {
    Q_OBJECT

public:
    explicit PlotHelper(QFrame *frame, QObject* parent = nullptr);

    void add_series(const std::string &id, Curve curve = {});
    void update_series_style(const std::string &id, const Curve &config);

    [[nodiscard]] auto plot() const -> QwtPlot *;

    void auto_scale();
    void show_context_menu(const QPoint& pos);

private:
    QwtPlot *m_plot;
    QwtPlotGrid *m_grid;
    QwtPlotZoomer *m_zoomer;
    QwtPlotMagnifier *m_magnifier;
    std::map<std::string, QwtPlotCurve*> m_curves;
};

} // namespace plot

#endif // PLOTHELPER_H