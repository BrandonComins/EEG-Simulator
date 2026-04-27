#include "mainwindow.h"
#include "connectiondialog.h"
#include "plothelper.h"
#include "ui_mainwindow.h"
#include "packettransceiver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_poll_timer(new QTimer(this))
    , m_plot(nullptr)
    , m_server(new QTcpServer(this))
    , m_current_client(nullptr)
    , m_packet_transceiver(nullptr)
    , m_connection_dialog(new ConnectionDialog(this))
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    constexpr int poll_time_ms = 20;
    m_poll_timer->setInterval(poll_time_ms);

    m_plot = new Plot::PlotHelper(ui->frame, this);

    m_server->setMaxPendingConnections(1);

    QObject::connect(m_poll_timer, &QTimer::timeout, this, &MainWindow::request_latest_data);

    QObject::connect(ui->action_connect, &QAction::triggered,
                     m_connection_dialog, &ConnectionDialog::show);

    QObject::connect(m_connection_dialog, &ConnectionDialog::start_server_requested,
                     this, [&](int port) {
                         if (!m_server->isListening()) {
                             m_server->listen(QHostAddress::Any, port);
                         }
                     });

    QObject::connect(m_connection_dialog, &ConnectionDialog::stop_server_requested,
                     this, [&]() {
                        m_server->close();

                        if(m_current_client) {
                            m_current_client->disconnectFromHost();
                        }
                     });

    QObject::connect(m_server, &QTcpServer::newConnection, this, &MainWindow::handle_new_connection);
}

void MainWindow::handle_new_connection() {
    QTcpSocket* socket = m_server->nextPendingConnection();

    if (socket) {
        m_current_client = socket;

        if (m_packet_transceiver) {
            m_packet_transceiver->deleteLater();
        }

        m_packet_transceiver = new Communication::PacketTransceiver(socket, this);

        connect(m_packet_transceiver, &Communication::PacketTransceiver::packet_received,
                this, &MainWindow::process_incoming_packet);

        m_connection_dialog->user_connected();
        m_poll_timer->start();

        QObject::connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            m_connection_dialog->user_disconnected();
            m_poll_timer->stop();

            if (m_current_client == socket) {
                m_current_client = nullptr;
                m_packet_transceiver = nullptr;
            }

            socket->deleteLater();
        });
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::MainWindow::process_incoming_packet(const QByteArray &data) {
    constexpr int num_channels = 11;
    if (static_cast<size_t>(data.size()) == sizeof(Communication::EEGDataReply)){
        const auto* reply = reinterpret_cast<const Communication::EEGDataReply*>(data.constData());
        double x = static_cast<double>(reply->header.timestamp_ms) / 1000.0;

        for (int channel = 0; channel < num_channels; ++channel) {
            std::string channel_id = "Channel " + std::to_string(channel);
            double y = static_cast<double>(reply->channels[channel].value);

            m_plot->add_point(channel_id, x, y);
        }
    }
}

void MainWindow::request_latest_data() {
    if (m_current_client) {
        m_packet_transceiver->send_command(Communication::CMD_GET_LATEST_DATA, nullptr, 0);
    }
}