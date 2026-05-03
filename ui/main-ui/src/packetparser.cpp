#include "packetparser.h"

#include "fmt/base.h"
#include "plotdefs.h"

#include <QTcpSocket>

PacketParser::PacketParser(QObject *parent)
    : QObject(parent)
    , m_num_channels(0) {

    /*empty*/
}

void PacketParser::process_incoming_packet(const QByteArray &data) {
    if (data.size() >=
        static_cast<int>(sizeof(Communication::PacketHeader) + sizeof(Communication::OPCode))) {
        const auto *header =
            reinterpret_cast<const Communication::PacketHeader *>(data.constData());

        if (header->type == Communication::PACKET_TYPE_REPLY) {
            auto cmd =
                static_cast<Communication::OPCode>(data.at(sizeof(Communication::PacketHeader)));
            const char *payload = data.constData() + sizeof(Communication::PacketHeader) +
                                  sizeof(Communication::OPCode);

            switch (cmd) {
            case Communication::CMD_GET_LATEST_DATA:
                handle_eeg_data(header, payload);
                break;

            case Communication::CMD_GET_NUM_CHANNELS:
                handle_channel_count(payload);
                break;

            case Communication::CMD_GET_CHANNEL_CONSTANTS:
                handle_channel_constants(payload);
                break;

            default:
                fmt::println("UI received reply for unhandled command:", static_cast<uint8_t>(cmd));
                break;
            }
        }
    }
}

std::string get_channel_name(int channel_id) {
    return "Channel " + std::to_string(channel_id);
}

void PacketParser::handle_eeg_data(const Communication::PacketHeader *header, const char *payload) {
    const auto *sample = reinterpret_cast<const Communication::EEGSample *>(payload);
    constexpr double ms_to_s = 1000.0;

    const auto x = static_cast<double>(header->timestamp_ms) / ms_to_s;
    const auto y = static_cast<double>(sample->value);

    Q_EMIT got_channel_data(get_channel_name(sample->channel_id), x, y);
}

void PacketParser::handle_channel_count(const char *payload) {
    const auto *reply = reinterpret_cast<const Communication::ChannelsCount *>(payload);

    m_num_channels = static_cast<int>(reply->num_channels);
    fmt::println("UI synced: Hardware reporting {} channels.", m_num_channels);

    for (int channel = 0; channel < m_num_channels; ++channel) {
        constexpr int y_offset = 100;
        Plot::CurveConfig config;
        config.y_offset = channel * y_offset;

        config.color = Plot::default_palette.at(channel);

        Q_EMIT got_new_channel(get_channel_name(channel), config);
    }
}

void PacketParser::handle_channel_constants(const char *payload) {
    Communication::ChannelConstants reply;
    std::memcpy(&reply, payload, sizeof(Communication::ChannelConstants));

    Q_EMIT got_channel_constants(reply);
}
