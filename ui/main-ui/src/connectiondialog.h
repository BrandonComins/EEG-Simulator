#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConnectionDialog;
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
    Ui::ConnectionDialog *ui;    //!< Pointer to the Ui.
};

#endif // CONNECTIONDIALOG_H