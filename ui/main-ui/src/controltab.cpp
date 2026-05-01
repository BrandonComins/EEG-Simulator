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

    QObject::connect(ui->pushButton_sync, &QPushButton::clicked,
                     this, qOverload<>(&ControlTab::request_channel_constants));

    QObject::connect(ui->pushButton_send, &QPushButton::clicked, this,
                     &ControlTab::send_channel_constants);
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

    request_channel_constants(widget->channel_id());
}

void ControlTab::add_transceiver(Communication::PacketTransceiver *packet_transceiver) {
    m_packet_transceiver = packet_transceiver;
}

void ControlTab::update_channel_constants(const Communication::ChannelConstants &constants) {
    if (constants.channel_id < m_control_widgets.size()) {
        auto &channel = m_control_widgets.at(constants.channel_id);

        channel->set_alpha_amplitude_uv(constants.alpha_amplitude);
        channel->set_alpha_frequency_hz(constants.alpha_frequency);
        channel->set_beta_amplitude_uv(constants.beta_amplitude);
        channel->set_beta_frequency_hz(constants.beta_frequency);
        channel->set_noise_scale(constants.noise_level);
        channel->set_noise_persistence(constants.noise_persistence);
    }
}


void ControlTab::on_connection() {
    request_num_channels();
    m_poll_timer->start();
}

void ControlTab::on_disconnect() {
    m_poll_timer->stop();
}

void ControlTab::send_channel_constants() {
    if(m_packet_transceiver) {
        for(auto const &channel : std::as_const(m_control_widgets)) {
            Communication::ChannelConstants pkt;
            pkt.channel_id = channel->channel_id();
            pkt.alpha_amplitude = channel->alpha_amplitude_uv();
            pkt.alpha_frequency = channel->alpha_frequency();
            pkt.beta_amplitude = channel->beta_amplitude_uv();
            pkt.beta_frequency = channel->beta_frequency_hz();
            pkt.noise_level = channel->noise_scale();
            pkt.noise_persistence = channel->noise_persistence();

            m_packet_transceiver->send_command(Communication::CMD_SET_CHANNEL_CONSTANTS, pkt);
        }
    }
}

void ControlTab::request_channel_constants() {
    if (m_packet_transceiver) {
        for(auto const &channel : std::as_const(m_control_widgets)) {
            request_channel_constants(channel->channel_id());
        }
    }
}

void ControlTab::request_channel_constants(int channel_id) {
    Communication::RequestChannelConstants pkt;
    pkt.channel_id = channel_id;
    m_packet_transceiver->send_command(Communication::CMD_GET_CHANNEL_CONSTANTS, pkt);
}

