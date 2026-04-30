#ifndef PLOTHELPER_H
#define PLOTHELPER_H

#include "multiseriestracker.h"
#include "plotdefs.h"
#include <QObject>
#include <QVector>
#include <string>

class QFrame;
class QwtPlot;
class QwtPlotCurve;
class QwtPlotGrid;
class QwtPlotLegendItem;
class QwtPlotMagnifier;
class QwtPlotZoomer;
class QwtPlotPicker;

namespace Plot {

class StyleCurveDiag;
class ScaleDialog;

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
    void set_view_mode(PlotMode view_mode);

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
     * \brief Show or hide the plot axes
     * \param x_axis_visible True to enable the bottom axis.
     * \param y_axis_visible True to enable the left axis.
     */
    void set_axis_visible(bool x_axis_visible, bool y_axis_visible);

    /*!
     * \brief Show or hide the plot axis titles
     * \param x_title_visible True to display the X-axis title.
     * \param y_title_visible True to display the Y-axis title.
     */
    void set_axis_titles_visible(bool x_title_visible, bool y_title_visible);

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

    /*!
     * \brief Enable or disable the X-axis (bottom)
     * \param visible True to show the axis, false to hide it.
     */
    void set_x_axis_visible(bool visible);

    /*!
     * \brief Enable or disable the Y-axis (left)
     * \param visible True to show the axis, false to hide it.
     */
    void set_y_axis_visible(bool visible);

    /*!
     * \brief Toggle the visibility of the X-axis title
     * \param visible True to display the title, false to clear it.
     */
    void set_x_axis_title_visible(bool visible);

    /*!
     * \brief Toggle the visibility of the Y-axis title
     * \param visible True to display the title, false to clear it.
     */
    void set_y_axis_title_visible(bool visible);

    /*!
     * \brief Save plot data or visuals
     * \param file_type The format to export (e.g., CSV or IMAGE).
     */
    void save_file(FileType file_type);

    /*!
     * \brief Export all active series data to a CSV file
     * \param path System path for the output file.
     */
    void export_csv(const QString &path);

    /*!
     * \brief Save the current plot view as an image
     * \param path System path for the output image.
     */
    void export_image(const QString &path);

    bool m_paused;                      //!< Flag for if the plot is paused
    bool m_auto_scale_x;                //!< Flag for X-axis auto-scaling.
    bool m_auto_scale_y;                //!< Flag for Y-axis auto-scaling.
    QString m_x_axis_title;             //!< Title of the X-axis
    QString m_y_axis_title;             //!< Title of the Y-axis
    PlotMode m_view_mode;               //!< Current horizontal viewing behavior.
    ScaleDialog *m_scale_diag;          //!< Dialog for manual scale input.
    QwtPlot *m_plot;                    //!< The core QwtPlot widget.
    QwtPlotLegendItem *m_legend;        //!< The legend of the plot.
    QwtPlotGrid *m_grid;                //!< Visual background grid.
    QwtPlotZoomer *m_zoomer;            //!< Zooming interaction handler.
    QwtPlotMagnifier *m_magnifier;      //!< Mouse-wheel magnification handler.
    MultiSeriesTracker *m_picker;       //!< Displays the point at a picked position
    std::vector<std::string> m_series_order; //!< The order of insertion for the series
    std::map<std::string, Series> m_series; //!< Map of IDs to data series.
    std::map<std::string, StyleCurveDiag*> m_style_dialogs; //!< Map of IDs to data series dialogs.
};

} // namespace Plot

#endif // PLOTHELPER_H