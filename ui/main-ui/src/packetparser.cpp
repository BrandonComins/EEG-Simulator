#include "packetparser.h"
#include <QDebug>
#include "plotdefs.h"

PacketParser::PacketParser(QObject *parent)
    : QObject(parent)
    , m_num_channels(0) {

    /*empty*/
}

void PacketParser::process_incoming_packet(const QByteArray &data) {
    if (data.size() >= static_cast<int>(sizeof(Communication::PacketHeader) + sizeof(Communication::OPCode))) {
        const auto* header = reinterpret_cast<const Communication::PacketHeader*>(data.constData());

        if (header->type == Communication::PACKET_TYPE_REPLY) {
            auto cmd = static_cast<Communication::OPCode>(data.at(sizeof(Communication::PacketHeader)));
            const char* payload_ptr = data.constData() + sizeof(Communication::PacketHeader) + sizeof(Communication::OPCode);

            switch (cmd) {
            case Communication::CMD_GET_LATEST_DATA:
                handle_eeg_data(header, payload_ptr);
                break;

            case Communication::CMD_GET_NUM_CHANNELS:
                handle_channel_count(payload_ptr);
                break;

            default:
                qDebug() << "UI received reply for unhandled command:" << static_cast<uint8_t>(cmd);
                break;
            }
        }
    }
}

std::string get_channel_name(int channel_id) {
    return "Channel " + std::to_string(channel_id);
}

void PacketParser::handle_eeg_data(const Communication::PacketHeader *header, const char *payload) {
    const auto* sample = reinterpret_cast<const Communication::EEGSample*>(payload);

    const auto x = static_cast<double>(header->timestamp_ms) / 1000.0;
    const auto y = static_cast<double>(sample->value);

    Q_EMIT got_channel_data(get_channel_name(sample->channel_id), x, y);
}

void PacketParser::handle_channel_count(const char *payload) {
    const auto* reply = reinterpret_cast<const Communication::ChannelsCount*>(payload);

    m_num_channels = static_cast<int>(reply->num_channels);
    qDebug() << "UI synced: Hardware reporting" << m_num_channels << "channels.";

    for(int channel = 0; channel < m_num_channels; ++channel) {
        constexpr int y_offset = 50;
        Plot::CurveConfig config;
        config.y_offset = channel * y_offset;

        config.color = Plot::default_palette.at(channel);

        Q_EMIT got_new_channel(get_channel_name(channel), config);
    }
}
