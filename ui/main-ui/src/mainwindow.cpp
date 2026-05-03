#include "mainwindow.h"

#include "connectiondialog.h"
#include "fmt/base.h"
#include "packetparser.h"
#include "packettransceiver.h"
#include "plothelper.h"
#include "ui_mainwindow.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_plot(nullptr)
    , m_server(new QTcpServer(this))
    , m_current_client(nullptr)
    , m_packet_transceiver(nullptr)
    , m_packet_parser(new PacketParser(this))
    , m_connection_dialog(new ConnectionDialog(this))
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_plot = new Plot::PlotHelper(ui->frame, this);
    m_plot->set_view_mode(Plot::ROLLING);
    m_plot->set_scale_x(0, 5);
    m_plot->set_scale_y(-50, 1050);

    ui->action_connect->setShortcut(QKeySequence("Ctrl+C"));
    ui->action_quick_start->setShortcut(QKeySequence("Ctrl+Shift+C"));

    QObject::connect(ui->action_quick_start, &QAction::triggered, m_connection_dialog,
                     &ConnectionDialog::toggle_server);

    QObject::connect(ui->action_connect, &QAction::triggered, m_connection_dialog,
                     &ConnectionDialog::show, Qt::UniqueConnection);

    QObject::connect(m_connection_dialog, &ConnectionDialog::start_server_requested, this,
                     [&](int port) {
                         if (!m_server->isListening()) {
                             if (m_server->listen(QHostAddress::Any, port)) {
                                 fmt::println("Server listening on port {}", port);
                             }
                         }
                     });

    QObject::connect(m_connection_dialog, &ConnectionDialog::stop_server_requested, this, [&]() {
        m_server->close();
        if (m_current_client) {
            m_current_client->disconnectFromHost();
        }
    });

    QObject::connect(m_server, &QTcpServer::newConnection, this, &MainWindow::handle_new_connection,
                     Qt::UniqueConnection);

    QObject::connect(m_packet_parser, &PacketParser::got_new_channel, this,
                     [&](const std::string &channel_id, const Plot::CurveConfig &config) {
                         ui->widget_control_tab->add_channel(channel_id);
                         m_plot->add_series(channel_id, config);
                     });

    QObject::connect(m_packet_parser, &PacketParser::got_channel_data, m_plot,
                     &Plot::PlotHelper::add_point, Qt::UniqueConnection);

    QObject::connect(m_packet_parser, &PacketParser::got_channel_constants, ui->widget_control_tab,
                     &ControlTab::update_channel_constants, Qt::UniqueConnection);
}

MainWindow::~MainWindow() {
    if (m_server) {
        m_server->close();
    }

    if (m_current_client) {
        m_current_client->disconnect();
        m_current_client->abort();
    }

    delete ui;
}

void MainWindow::handle_new_connection() {
    QTcpSocket *socket = m_server->nextPendingConnection();

    if (socket) {
        m_current_client = socket;

        if (m_packet_transceiver) {
            m_packet_transceiver->deleteLater();
        }

        m_packet_transceiver = new Communication::PacketTransceiver(socket, this);
        ui->widget_control_tab->add_transceiver(m_packet_transceiver);

        QObject::connect(m_packet_transceiver, &Communication::PacketTransceiver::packet_received,
                         this, [this](const QByteArray &packet) {
                             m_packet_parser->process_incoming_packet(packet);
                             m_connection_dialog->update_byte_count(packet.size());
                         });

        m_connection_dialog->user_connected();

        ui->widget_control_tab->on_connection();

        QObject::connect(socket, &QTcpSocket::disconnected, this, [this, socket]() {
            if(m_connection_dialog) {
                m_connection_dialog->user_disconnected();
            }

            if (m_current_client == socket) {
                m_current_client = nullptr;
                m_packet_transceiver = nullptr;
            }
            socket->deleteLater();

            if (ui && ui->widget_control_tab) {
                ui->widget_control_tab->on_disconnect();
            }
        });
    }
}
