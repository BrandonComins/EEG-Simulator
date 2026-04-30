#include "controltab.h"

#include "controlwidget.h"
#include "packets.h"
#include "packettransceiver.h"
#include "ui_controltab.h"

#include <QTimer>

ControlTab::ControlTab(QWidget *parent)
    : QWidget(parent)
    , m_poll_timer(new QTimer(this))
    , m_packet_transceiver(nullptr)
    , ui(new Ui::ControlTab)
{
    ui->setupUi(this);

    constexpr int poll_time_ms = 20;
    m_poll_timer->setInterval(poll_time_ms);

    QObject::connect(m_poll_timer, &QTimer::timeout, this,
                     &ControlTab::request_latest_data, Qt::UniqueConnection);
}

ControlTab::~ControlTab() {
    delete ui;
}

void ControlTab::request_latest_data() {
    if (m_packet_transceiver) {
        for (uint8_t channel = 0; channel < m_control_widgets.size(); ++channel) {
            Communication::RequestSample request;
            request.channel_id = channel;
            m_packet_transceiver->send_command(Communication::CMD_GET_LATEST_DATA, request);
        }
    }
}

void ControlTab::request_num_channels() {
    if (m_packet_transceiver) {
        m_packet_transceiver->send_command(Communication::CMD_GET_NUM_CHANNELS);
    }
}

void ControlTab::add_channel(const std::string &control_id) {
    auto widget = new ControlWidget(control_id, this);

    m_control_widgets.push_back(widget);
    ui->gridLayout_widgets->addWidget(widget);
}

void ControlTab::add_transceiver(Communication::PacketTransceiver *packet_transceiver) {
    m_packet_transceiver = packet_transceiver;
}

void ControlTab::on_connection() {
    request_num_channels();
    m_poll_timer->start();
}

void ControlTab::on_disconnect() {
    m_poll_timer->stop();
}

