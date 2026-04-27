#include "commandparser.h"
#include "eeg.h"
#include "packets.h"
#include "packettransceiver.h"

#include <QDebug>

Communication::CommandParser::CommandParser(EEG *simulator, PacketTransceiver *transceiver)
    : m_eeg(simulator)
    , m_transceiver(transceiver){

    /*empty*/
}

void Communication::CommandParser::process_raw_packet(const QByteArray &data) {
    const auto* request = reinterpret_cast<const GenericCommandRequest*>(data.constData());

    this->process_request(*request);
}

bool Communication::CommandParser::process_request(const GenericCommandRequest &request) {
    switch (request.command_id) {

    case CMD_SET_ALPHA_PARAMS: {
        const auto* cfg = reinterpret_cast<const AlphaConfig*>(request.payload);
        m_eeg->set_alpha_params(cfg->amplitude, cfg->frequency);

        m_transceiver->send_reply(CMD_SET_ALPHA_PARAMS, nullptr, 0);
        return true;
    }

    case CMD_SET_BETA_PARAMS: {
        const auto* cfg = reinterpret_cast<const BetaConfig*>(request.payload);
        m_eeg->set_beta_params(cfg->amplitude, cfg->frequency);

        m_transceiver->send_reply(CMD_SET_BETA_PARAMS, nullptr, 0);
        return true;
    }

    case CMD_SET_NOISE_LEVEL: {
        const auto* cfg = reinterpret_cast<const NoiseConfig*>(request.payload);
        m_eeg->set_noise_level(cfg->scale);

        m_transceiver->send_reply(CMD_SET_NOISE_LEVEL, nullptr, 0);
        return true;
    }

    case CMD_GET_LATEST_DATA: {
        constexpr int num_chanels = 11;
        constexpr int ms_to_s = 1000;

        EEGSample samples[num_chanels];
        double timestamp_secs = m_transceiver->get_timestamp() / ms_to_s;

        m_eeg->fill_latest_samples(samples, num_chanels, timestamp_secs);

        m_transceiver->send_reply(CMD_GET_LATEST_DATA, samples, sizeof(samples));

        return true;
    }

    default:
        qDebug() << request.command_id << "is not a valid command";
        return false;
    }
}