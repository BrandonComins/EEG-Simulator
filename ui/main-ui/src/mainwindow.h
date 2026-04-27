#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtcpserver.h>

class ConnectionDialog;

namespace Plot{
class PlotHelper;
}

namespace Communication{
class PlotHelper;
class PacketTransceiver;
}

namespace Ui {
class MainWindow;
}

/*!
 * \class MainWindow
 * \brief The central widget of the UI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:

    /*!
     * \brief Constructs the MainWindow.
     * \param parent Pointer to the parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /*!
     * \brief Destructor for the Ui main window.
     */
    ~MainWindow();

    /*!
     * \brief Unpacks and routes incoming binary data.
     * \param data The raw, validated binary packet from the transceiver.
     */
    void process_incoming_packet(const QByteArray &data);

  private:

    /*!
     * \brief Requests the latest EEG frame.
     */
    void request_latest_data();

    /*!
     * \brief Handles the handshake when a new Simulator connects to the server.
     */
    void handle_new_connection();

    QTimer *m_poll_timer;         //!< Timer to request new samples
    Plot::PlotHelper *m_plot;     //!< Plot For the EEG samples
    QTcpServer *m_server;         //!< The server
    QTcpSocket* m_current_client; //!< The client connected
    Communication::PacketTransceiver *m_packet_transceiver; //!< The packet sender / receiver
    ConnectionDialog *m_connection_dialog; //!< The conenction dialog
    Ui::MainWindow *ui;                    //!< Qt Ui Object
};

#endif // MAINWINDOW_H
