#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class ConnectionDialog;
class QTimer;
class QTcpSocket;
class QTcpServer;
class PacketParser;

namespace Plot {
class PlotHelper;
}

namespace Communication {
class PacketTransceiver;
}

namespace Ui {
class MainWindow;
}

/*!
 * \class MainWindow
 * \brief The central widget of the UI.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private:
    /*!
     * \brief Handles the handshake when a new Simulator connects to the server.
     */
    void handle_new_connection();

    Plot::PlotHelper *m_plot;                               //!< Plot for the EEG samples
    QTcpServer *m_server;                                   //!< The TCP server
    QTcpSocket *m_current_client;                           //!< The currently connected client
    Communication::PacketTransceiver *m_packet_transceiver; //!< The packet handler
    PacketParser *m_packet_parser;                          //!< Pointer to the packet parser
    ConnectionDialog *m_connection_dialog;                  //!< The connection dialog
    Ui::MainWindow *ui;                                     //!< Qt UI Object
};

#endif // MAINWINDOW_H