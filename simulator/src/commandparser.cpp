#include "commandparser.h"

#include "eeg.h"
#include "fmt/base.h"
#include "fmt/ranges.h"
#include "packets.h"
#include "packettransceiver.h"
#include "spdlog/spdlog.h"

namespace Communication {

CommandParser::CommandParser(const std::vector<EEG *> &channels, PacketTransceiver *transceiver)
    : m_log_buffer(channels.size())
    , m_transceiver(transceiver)
    , m_channels(channels) {
    /* empty */
}

void CommandParser::process_raw_packet(const QByteArray &data) {
    if (data.size() >= static_cast<int>(sizeof(PacketHeader) + sizeof(OPCode))) {
        const auto *header = reinterpret_cast<const PacketHeader *>(data.constData());
        const auto cmd = static_cast<OPCode>(data.at(sizeof(PacketHeader)));
        const char *payload_ptr = data.constData() + sizeof(PacketHeader) + sizeof(OPCode);

        this->execute_command(cmd, *header, payload_ptr);
    }
}

bool CommandParser::execute_command(OPCode cmd, const PacketHeader &header, const char *payload) {
    const uint8_t req_id = header.request_id;

    switch (cmd) {

    case CMD_SET_CHANNEL_CONSTANTS: {
        const auto *pkt = reinterpret_cast<const ChannelConstants *>(payload);
        auto &channel = m_channels.at(pkt->channel_id);

        channel->set_alpha_amplitude(pkt->alpha_amplitude);
        channel->set_alpha_frequency(pkt->alpha_frequency);
        channel->set_beta_amplitude(pkt->beta_amplitude);
        channel->set_beta_frequency(pkt->beta_frequency);
        channel->set_noise_level(pkt->noise_level);
        channel->set_noise_persistence(pkt->noise_persistence);

        return true;
    }

    case CMD_GET_CHANNEL_CONSTANTS: {
        const auto *pkt = reinterpret_cast<const RequestChannelConstants *>(payload);
        auto &channel = m_channels.at(pkt->channel_id);

        ChannelConstants reply;
        reply.channel_id = pkt->channel_id;
        reply.alpha_amplitude = channel->alpha_amplitude_uv();
        reply.alpha_frequency = channel->alpha_freq_hz();
        reply.beta_amplitude = channel->beta_amplitude_uv();
        reply.beta_frequency = channel->beta_freq_hz();
        reply.noise_persistence = channel->noise_persistence();
        reply.noise_level = channel->noise_level();

        m_transceiver->send_reply(cmd, req_id, reply);

        return true;
    }

    case CMD_GET_NUM_CHANNELS: {
        ChannelsCount data;
        data.num_channels = static_cast<uint8_t>(m_channels.size());

        m_transceiver->send_reply(cmd, req_id, data);

        return true;
    }

    case CMD_GET_LATEST_DATA: {
        const auto *req = reinterpret_cast<const RequestSample *>(payload);
        uint8_t channel = req->channel_id;

        if (channel >= m_channels.size()) {
            fmt::println("UI requested invalid channel: {}", channel);
            return false;
        }

        constexpr double ms_to_s = 1000.0;
        constexpr int sample_quality = 100;
        const double timestamp_secs = static_cast<double>(m_transceiver->get_timestamp()) / ms_to_s;

        EEGSample reply;
        reply.value = m_channels[channel]->get_next_sample(timestamp_secs);
        reply.quality = sample_quality;
        reply.channel_id = channel;

        m_log_buffer[channel] = reply.value;

        if (channel == m_channels.size() - 1) {
            spdlog::info("{:.4f}, {}", timestamp_secs, fmt::join(m_log_buffer, ", "));
        }

        m_transceiver->send_reply(cmd, req_id, reply);

        return true;
    }

    default:
        fmt::println("Received unknown Command ID: {}", static_cast<int>(cmd));
        return false;
    }

    return false;
}

} // namespace Communication