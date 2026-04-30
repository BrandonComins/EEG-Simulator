#include "commandparser.h"
#include "eeg.h"
#include "packets.h"
#include "packettransceiver.h"

#include <QDebug>

namespace Communication {

CommandParser::CommandParser(std::vector<EEG> &channels, PacketTransceiver *transceiver)
    : m_transceiver(transceiver)
    , m_channels(channels) {
    /* empty */
}

void CommandParser::process_raw_packet(const QByteArray &data) {
    if (data.size() >= static_cast<int>(sizeof(PacketHeader) + sizeof(OPCode))) {
        const auto *header = reinterpret_cast<const PacketHeader*>(data.constData());
        const auto cmd = static_cast<OPCode>(data.at(sizeof(PacketHeader)));
        const char *payload_ptr = data.constData() + sizeof(PacketHeader) + sizeof(OPCode);

        this->execute_command(cmd, *header, payload_ptr);
    }
}

bool CommandParser::execute_command(OPCode cmd, const PacketHeader& header, const char* payload) {
    const uint8_t req_id = header.request_id;

    switch (cmd) {

    case CMD_SET_ALPHA_AMPLITUDE: {
        const auto* pkt = reinterpret_cast<const Amplitude*>(payload);
        if (pkt->channel_id < m_channels.size()) {
            m_channels.at(pkt->channel_id).set_alpha_amplitude(pkt->amplitude);
            m_transceiver->send_reply(cmd, req_id, GenericAck{cmd, 0});

            return true;
        }
        break;
    }

    case CMD_SET_ALPHA_FREQUENCY: {
        const auto* pkt = reinterpret_cast<const Frequency*>(payload);
        if (pkt->channel_id < m_channels.size()) {
            m_channels.at(pkt->channel_id).set_alpha_frequency(pkt->frequency);
            m_transceiver->send_reply(cmd, req_id, GenericAck{cmd, 0});

            return true;
        }
        break;
    }

    case CMD_SET_NOISE_SCALE: {
        const auto* pkt = reinterpret_cast<const NoiseScale*>(payload);
        if (pkt->channel_id < m_channels.size()) {
            m_channels.at(pkt->channel_id).set_noise_level(pkt->scale);
            m_transceiver->send_reply(cmd, req_id, GenericAck{cmd, 0});

            return true;
        }
        break;
    }

    case CMD_GET_NUM_CHANNELS: {
        ChannelsCount data;
        data.num_channels = static_cast<uint8_t>(m_channels.size());
        m_transceiver->send_reply(cmd, req_id, data);

        return true;
    }

    case CMD_GET_LATEST_DATA: {
        const auto* req = reinterpret_cast<const RequestSample*>(payload);
        uint8_t ch = req->channel_id;

        if (ch >= m_channels.size()) {
            qWarning() << "UI requested invalid channel:" << ch;
            return false;
        }

        constexpr double ms_to_s = 1000.0;
        const double timestamp_secs = static_cast<double>(m_transceiver->get_timestamp()) / ms_to_s;
        constexpr int sample_quality = 100; //Don't hardcode me later

        EEGSample sample;
        sample.value = m_channels[ch].get_next_sample(timestamp_secs);
        sample.quality = sample_quality;
        sample.channel_id = ch;

        m_transceiver->send_reply(cmd, req_id, sample);
        return true;
    }

    default:
        qWarning() << "Received unknown Command ID:" << static_cast<uint8_t>(cmd);
        return false;
    }

    return false;
}

} // namespace Communication