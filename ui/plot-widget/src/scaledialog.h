#ifndef SCALEDIALOG_H
#define SCALEDIALOG_H

#include <QDialog>

namespace Ui {
class ScaleDialog;
}

namespace Plot {

class ScaleDialog : public QDialog {
    Q_OBJECT

  public:
    /*!
     * \brief Constructs a new ScaleDialog object.
     * \param parent The parent widget.
     */
    explicit ScaleDialog(QWidget *parent = nullptr);

    /*!
     * \brief Destroys the ScaleDialog.
     */
    ~ScaleDialog() override;

    /*!
     * \brief Collects values from the UI fields and emits the scale_clicked signal.
     */
    void handle_scale_button();

  Q_SIGNALS:
    /*!
     * \brief Signal emitted when the user confirms manual scale values.
     * \param min_x The minimum value for the horizontal axis.
     * \param max_x The maximum value for the horizontal axis.
     * \param min_y The minimum value for the vertical axis.
     * \param max_y The vertical axis maximum value.
     */
    void scale_clicked(double min_x, double max_x, double min_y, double max_y);

    /*!
     * \brief Signal emitted when the user requests an automatic best-fit scaling.
     */
    void auto_scale_clicked();

  private:
    Ui::ScaleDialog *ui; //!< Pointer to the UI object
};
} // namespace Plot

#endif // SCALEDIALOG_H
