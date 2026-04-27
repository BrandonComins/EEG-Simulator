#include "packettransceiver.h"
#include "packets.h"

#include <chrono>
#include <qdebug.h>

Communication::PacketTransceiver::PacketTransceiver(QIODevice *device, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_last_request_id(0)
    , m_global_packet_count(0) {

    QObject::connect(m_device, &QIODevice::readyRead, this, &PacketTransceiver::read_data);
}

void Communication::PacketTransceiver::send_command(CommandID cmd, const void *data, size_t data_size) {
    GenericCommandRequest request;

    request.header.type = PACKET_TYPE_REQUEST;
    request.header.request_id = ++m_last_request_id;
    request.header.packet_id = ++m_global_packet_count;
    request.header.timestamp_ms = get_timestamp();

    request.command_id = cmd;
    std::memset(request.payload, 0, sizeof(request.payload));

    if (data && data_size <= sizeof(request.payload)) {
        std::memcpy(request.payload, data, data_size);
    } else if (data_size > sizeof(request.payload)) {
        qDebug() << "Packet Payload Exceeds max size";
        return;
    }

    send_packet(request);
}

void Communication::PacketTransceiver::send_reply(CommandID cmd, const void *data, size_t data_size) {
    if (cmd == CMD_GET_LATEST_DATA) {
        EEGDataReply reply;
        prepare_reply_header(reply, cmd);
        if (data && data_size == sizeof(reply.channels)) {
            std::memcpy(reply.channels, data, data_size);
        }
        send_packet(reply);
    } else {
        GenericStatusReply reply;
        prepare_reply_header(reply, cmd);
        send_packet(reply);
    }
}

uint16_t Communication::PacketTransceiver::PacketTransceiver::calculate_checksum(const void *data, size_t length) {
    constexpr int footer_check_sum_length = 3;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    uint16_t checksum = 0;

    size_t target_length = length - footer_check_sum_length;

    for (size_t i = 0; i < target_length; ++i) {
        checksum += bytes[i];
    }

    return checksum;
}

bool Communication::PacketTransceiver::validate_checksum(const QByteArray &packet) {
    constexpr int footer_check_sum_length = 3;

    if (packet.size() > footer_check_sum_length) {
        uint16_t sent_checksum;
        std::memcpy(&sent_checksum, packet.data() + packet.size() - footer_check_sum_length, sizeof(uint16_t));

        uint16_t calculated = calculate_checksum(packet.data(), packet.size());

        return (sent_checksum == calculated);
    }

    return false;
}

void Communication::PacketTransceiver::read_data() {
    QByteArray buffer = m_device->readAll();
    m_incoming_data.append(buffer);
    process_buffer();
}

uint32_t Communication::PacketTransceiver::get_timestamp() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    return millis;
}

void Communication::PacketTransceiver::process_buffer() {
    while (m_incoming_data.size() >= HEADER_SYNC_SIZE) {

        if (static_cast<uint8_t>(m_incoming_data[0]) != SOF_MARKER) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        const uint8_t expected_size = static_cast<uint8_t>(m_incoming_data[1]);

        if (expected_size < MIN_PACKET_SIZE || expected_size > MAX_PACKET_SIZE) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        if (m_incoming_data.size() < expected_size) {
            return;
        }

        const uint8_t footer_byte = static_cast<uint8_t>(m_incoming_data[expected_size - 1]);
        if (footer_byte != EOF_MARKER) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        QByteArray packet_raw = m_incoming_data.left(expected_size);
        m_incoming_data.remove(0, expected_size);

        if (validate_checksum(packet_raw)) {
            const auto* packet_ptr = reinterpret_cast<const PacketHeader*>(packet_raw.data());

            if (packet_ptr->type == PACKET_TYPE_REQUEST) {
                m_last_request_id = packet_ptr->request_id;
            }

            Q_EMIT packet_received(packet_raw);
        }
    }
}
