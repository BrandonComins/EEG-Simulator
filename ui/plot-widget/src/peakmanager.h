#include <QColor>
#include <QObject>
#include <QPointF>

class QwtPlot;
class QwtPlotMarker;

namespace Plot {

/*!
 * \brief The PeakManager class handles the detection and visualization of local extrema.
 */
class PeakManager : public QObject {
    Q_OBJECT

  public:
    /*!
     * \brief Constructs a PeakManager for a specific plot.
     * \param plot Pointer to the QwtPlot to monitor and mark.
     */
    explicit PeakManager(QwtPlot *plot);

    /*!
     *  \brief Enables or disables the visualization of local maxima.
     */
    void set_peaks_enabled(bool enabled);

    /*!
     *  \brief Enables or disables the visualization of local minima.
     */
    void set_mins_enabled(bool enabled);

    /*!
     *  \brief Returns if this is currently tracking local maxima
     *  \return True if local maxima markers are currently enabled.
     */
    [[nodiscard]] auto peaks_enabled() const -> bool;

    /*!
     * \return True if local minima markers are currently enabled.
     */
    [[nodiscard]] auto mins_enabled() const -> bool;

    /*!
     * \brief Returns if the peak manager is currently enabled
     * \return True if either peaks or mins are currently enabled.
     */
    [[nodiscard]] auto enabled() const -> bool;

    /*!
     * \brief Scans all visible curves for extrema and refreshes markers.
     */
    void refresh_peaks();

  private:
    /*!
     *  \brief Deletes all active markers and detaches them from the plot.
     */
    void clear_markers();

    /*!
     * \brief Creates and attaches a new triangle marker to the plot.
     * \param point The (x, y) coordinates for the marker.
     * \param color The base color (usually matching the curve).
     * \param is_peak True for an upward triangle (max), false for downward (min).
     */
    void add_peak_marker(const QPointF &point, const QColor &color, bool is_peak);

    QwtPlot *m_plot;   //!< The target plot widget.
    bool m_show_peaks; //!< Toggle state for maxima.
    bool m_show_mins;  //!< Toggle state for minima.
    std::vector<QwtPlotMarker *>
        m_active_markers; //!< List of markers currently owned by this manager.
};

} // namespace Plot
