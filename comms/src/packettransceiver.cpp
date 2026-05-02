#include "packettransceiver.h"

#include "packets.h"

#include <chrono>

Communication::PacketTransceiver::PacketTransceiver(QIODevice *device, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_last_request_id(0)
    , m_global_packet_count(0) {

    QObject::connect(m_device, &QIODevice::readyRead, this, &PacketTransceiver::read_data);
}

uint16_t Communication::PacketTransceiver::PacketTransceiver::calculate_checksum(const void *data,
                                                                                 size_t length) {
    constexpr int footer_check_sum_length = 3;
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(data);
    uint16_t checksum = 0;

    size_t target_length = length - footer_check_sum_length;

    for (size_t i = 0; i < target_length; ++i) {
        checksum += bytes[i];
    }

    return checksum;
}

bool Communication::PacketTransceiver::validate_checksum(const QByteArray &packet) {
    constexpr int checksum_offset_from_end = 3;
    if (packet.size() > checksum_offset_from_end) {
        uint16_t sent_checksum;
        uint16_t calculated =
            calculate_checksum(packet.data(), packet.size() - checksum_offset_from_end);

        std::memcpy(&sent_checksum, packet.data() + packet.size() - checksum_offset_from_end,
                    sizeof(uint16_t));

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

void Communication::PacketTransceiver::send_command(OPCode cmd) {
    struct Empty {};
    send_command(cmd, Empty());
}

void Communication::PacketTransceiver::process_buffer() {
    while (m_incoming_data.size() >= static_cast<int>(sizeof(PacketHeader))) {
        const auto *header = reinterpret_cast<const PacketHeader *>(m_incoming_data.constData());

        // Make sure we found the header at the start of the frame and it is the expected byte
        if (static_cast<uint8_t>(header->sof) != static_cast<uint8_t>(SOF_MARKER)) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        const uint8_t expected_size = header->packet_length;

        // Make sure that the size is within the realm of possibility
        constexpr uint8_t absolute_min = sizeof(PacketHeader) + 1 + sizeof(PacketFooter);
        if (expected_size < absolute_min || expected_size > 255) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        // Entire packet hasn't arrived yet
        if (m_incoming_data.size() < expected_size) {
            return;
        }

        // Make sure the footer is at the end of the frame and is the expected Byte
        const uint8_t footer = static_cast<uint8_t>(m_incoming_data.back());
        if (footer != EOF_MARKER) {
            m_incoming_data.remove(0, 1);
            continue;
        }

        // Whole packet arrived, make sure checksum is good
        QByteArray packet_raw = m_incoming_data.left(expected_size);
        if (validate_checksum(packet_raw)) {

            if (header->type == PACKET_TYPE_REQUEST) {
                m_last_request_id = header->request_id;
            }

            m_incoming_data.remove(0, expected_size);

            Q_EMIT packet_received(packet_raw);
        } else {
            m_incoming_data.remove(0, 1);
        }
    }
}
