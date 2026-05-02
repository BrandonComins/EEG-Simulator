#ifndef MULTI_SERIES_TRACKER_H
#define MULTI_SERIES_TRACKER_H

#include <qwt_plot_picker.h>

namespace Plot {

/*!
 * \brief A tracker that snaps to the nearest curve point and displays all series values.
 */
class MultiSeriesTracker : public QwtPlotPicker {
  public:
    MultiSeriesTracker(QWidget *canvas);

  protected:
    /*!
     * \brief Intercepts movement to snap the cursor position to the nearest data point.
     */
    virtual void move(const QPoint &pos) override;

    /*!
     * \brief Generates text showing values for all series at the current snapped X.
     */
    virtual QwtText trackerTextF(const QPointF &pos) const override;
};
} // namespace Plot

#endif // MULTI_SERIES_TRACKER_H