#ifndef STYLECURVEDIAG_H
#define STYLECURVEDIAG_H

#include "plotdefs.h"

#include <QDialog>
#include <string>

namespace Ui {
class StyleCurveDiag;
}

namespace Plot {

/*!
 * \brief A dialog for modifying the visual properties of a series.
 *
 * This dialog allows users to adjust parameters such as color, line style,
 * and symbol types for a specific curve. It operates on a reference to a
 * live configuration object, providing real-time feedback through signals.
 */
class StyleCurveDiag : public QDialog {
    Q_OBJECT

  public:
    /*!
     * \brief Constructor
     * \param id The ID of the series.
     * \param live_config Reference to the current configuration being applied to the series.
     * \param parent Optional parent widget.
     */
    explicit StyleCurveDiag(const std::string &id, Plot::CurveConfig &live_config,
                            QWidget *parent = nullptr);

    /*!
     * \brief Destructor for Style Curve Dialog
     */
    ~StyleCurveDiag();

  Q_SIGNALS:
    /*!
     * \brief Signal emitted when any visual setting is modified in the UI.
     * \param id ID of the series.
     */
    void settings_changed(const std::string &id);

  private:
    /*!
     * \brief Updates the series' config and emits settings_changed.
     */
    void on_ui_changed();

    /*!
     * \brief Finalizes the changes and closes the dialog.
     */
    void on_save_clicked();

    /*!
     * \brief Reverts the series config to the previous values.
     */
    void on_cancel_clicked();

    /*!
     * \brief Synchronizes the UI widgets to reflect the current series config.
     */
    void sync_ui_to_config();

    Ui::StyleCurveDiag *ui;            //!< Pointer to the UI layout object
    std::string m_id;                  //!< Identifier for the target curve
    Plot::CurveConfig &m_live_config;  //!< Reference to the active configuration
    Plot::CurveConfig m_backup_config; //!< Copy of the config at launch for cancellation recovery
};

} // namespace Plot

#endif // STYLECURVEDIAG_H