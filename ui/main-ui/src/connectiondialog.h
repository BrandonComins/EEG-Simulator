#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

class QSettings;

namespace Ui {
class ConnectionDialog;
}

namespace Plot {
class PlotHelper;
}

/*!
 * \class ConnectionDialog
 * \brief UI component responsible for managing the TCP Server lifecycle and status.
 */
class ConnectionDialog : public QDialog
{
    Q_OBJECT

  public:
    /*!
     * \brief Constructs the ConnectionDialog.
     * \param parent Parent widget.
     */
    explicit ConnectionDialog(QWidget *parent = nullptr);

    /*!
     * \brief Destructor for the Conenction Dialog.
     */
    ~ConnectionDialog();

    /*!
     * \brief Updates the UI to reflect a successful client connection.
     */
    void user_connected();

    /*!
     * \brief Updates the UI to reflect a client disconnection.
     */
    void user_disconnected();

    /*!
     * \brief Logic to handle the Start/Stop button toggle.
     */
    void toggle_server();

    /*!
     * \brief Adds the size of a received packet to the total count.
     * \param bytes The bytes downloaded
     */
    void update_byte_count(int bytes);

    /*!
     * \brief Calculates KB/s and updates the plot.
     */
    void calculate_speed();

  Q_SIGNALS:
    /*!
     * \brief Emitted when the user requests to start the server.
     * \param port The port of the server
     */
    void start_server_requested(int port);

    /*!
     * \brief Emitted when the user requests to shut down the server.
     */
    void stop_server_requested();

  private:
    /*!
     * \brief Updates the stylesheet of the status LED.
     * \param connected If true, sets the LED to Green, Red otherwise.
     */
    void set_led_status(bool connected);

    bool m_server_on;            //!< Tracks whether the server listener is active.
    double m_last_x_axis_point;
    int m_bytes_received;
    QTimer *m_stats_timer;
    Plot::PlotHelper *m_plot;    //!< Connection plot
    QSettings *m_settings;       //!< Settings to remember the port
    Ui::ConnectionDialog *ui;    //!< Pointer to the Ui.
};

#endif // CONNECTIONDIALOG_H