#ifndef PLOTHELPER_H
#define PLOTHELPER_H

#include "qwt_plot_curve.h"
#include <QObject>
#include <QVector>
#include <qcolor.h>
#include <qwt_symbol.h>
#include <map>
#include <vector>
#include <string>

class QFrame;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotLegendItem;
class QwtPlotMagnifier;
class QwtPlotZoomer;

namespace Plot {

class ScaleDialog;

/*!
 * \brief Default color palette for curves when no color is specified.
 */
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

/*!
 * \brief Configuration settings for a specific plot curve.
 */
struct CurveConfig {
    bool use_antialiasing = true;               //!< Toggle anti-aliased rendering.
    float line_width = 1.5f;                    //!< Thickness of the curve line.
    double vertical_offset = 0.0;               //!< Constant offset added to Y values.
    double z_order = 0.0;                       //!< Rendering priority (higher is on top).
    QColor color = Qt::transparent;             //!< Curve color.
    Qt::PenStyle line_style = Qt::SolidLine;    //!< Style of the line (Solid, Dashed, etc).
    QwtPlotCurve::CurveStyle curve_style = QwtPlotCurve::Lines; //!< Qwt curve drawing style.
    QwtSymbol::Style symbol_type = QwtSymbol::NoSymbol;         //!< Type of point symbols.
};

/*!
 * \brief Container for curve data and its associated configuration.
 */
struct Series {
    QwtPlotCurve* curve = nullptr;  //!< Pointer to the Qwt curve object.
    CurveConfig config;                   //!< Style and rendering configuration.
    std::vector<double> x_data;     //!< Buffer for X-axis coordinates.
    std::vector<double> y_data;     //!< Buffer for Y-axis coordinates.
};

/*!
 * \brief Defines how the plot behaves as new data points are added.
 */
enum ViewMode {
    CUMULATIVE, //!< Shows all data, expanding the X-axis as needed.
    ROLLING,    //!< Maintains a fixed window width, shifting with new data.
    SWEEP,      //!< Clears/re-draws from left to right in discrete pages.
};

/*!
 * \brief Helper for managing QwtPlot complexity and signal visualization.
 */
class PlotHelper : public QObject {
    Q_OBJECT

public:
    /*!
     * \brief Constructs the PlotHelper.
     * \param frame The QFrame that will contain the QwtPlot.
     * \param parent Optional QObject.
     */
    explicit PlotHelper(QFrame *frame, QObject* parent = nullptr);

    /*!
     * \brief Cleans up plot resources and dialogs.
     */
    ~PlotHelper() override;

    /*!
     * \brief set_axis_titles Set the Axis Titles
     * \param x_title Title for the X axis
     * \param y_title Title for the Y Axis
     */
    void set_axis_titles(const std::string &x_title, const std::string &y_title);

    /*!
     * \brief Adds a new data series to the plot.
     * \param id Unique identifier for the series.
     * \param curve Configuration for the series style.
     */
    void add_series(const std::string &id, CurveConfig curve = {});

    /*!
     * \brief Updates the visual style of an existing series.
     */
    void update_series_style(const std::string &id, const CurveConfig &config);

    /*!
     * \brief Appends a new data point to a series.
     */
    void add_point(const std::string &id, double x, double y);

    /*!
     * \brief Sets the plot's horizontal behavior mode.
     */
    void set_view_mode(ViewMode view_mode);

    /*!
     * \brief Enables or disables automatic scaling for the X axis.
     */
    void set_auto_scale_x(bool auto_scale);

    /*!
     * \brief Enables or disables automatic scaling for the Y axis.
     */
    void set_auto_scale_y(bool auto_scale);

    /*!
     * \brief Manually sets the horizontal scale range.
     */
    void set_scale_x(double min, double max);

    /*!
     * \brief Manually sets the vertical scale range.
     */
    void set_scale_y(double min, double max);

    /*!
     * \brief Triggers a best-fit scaling for both axes based on current data.
     */
    void auto_scale();

    /*!
     * \brief set_legend_visible Sets the legend visible or not visible
     * \param visible True if the legend should be visible
     */
    void set_legend_visible(bool visible);

    /*!
     * \brief set_grid_visible Sets the grid visible
     * \param visible True if the grid should be visible
     */
    void set_grid_visible(bool visible);

    /*!
     * \brief Displays the custom plot context menu at the specified position.
     */
    void show_context_menu(const QPoint& pos);

protected:
    /*!
     * \brief Provides access to the underlying QwtPlot widget.
     */
    [[nodiscard]] auto plot() const -> QwtPlot*;

private:
    /*!
     * \brief Handler for the manual scale dialog confirmation.
     */
    void manual_scale(double min_x, double max_x, double min_y, double max_y);

    bool m_paused;                      //!< Flag for if the plot is paused
    bool m_auto_scale_x;                //!< Flag for X-axis auto-scaling.
    bool m_auto_scale_y;                //!< Flag for Y-axis auto-scaling.
    ViewMode m_view_mode;               //!< Current horizontal viewing behavior.
    ScaleDialog *m_scale_diag;          //!< Dialog for manual scale input.
    QwtPlot *m_plot;                    //!< The core QwtPlot widget.
    QwtPlotLegendItem *m_legend;                //!< The legend of the plot.
    QwtPlotGrid *m_grid;                //!< Visual background grid.
    QwtPlotZoomer *m_zoomer;            //!< Zooming interaction handler.
    QwtPlotMagnifier *m_magnifier;      //!< Mouse-wheel magnification handler.
    std::map<std::string, Series> m_series; //!< Map of IDs to data series objects.
};

} // namespace Plot

#endif // PLOTHELPER_H