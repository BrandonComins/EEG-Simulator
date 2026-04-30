#ifndef PACKET_TRANSCEIVER_H
#define PACKET_TRANSCEIVER_H

/*!
 * \class PacketTransceiver
 * \brief Handles the construction and parsing of packets over any I/O device.
 */
#include "packets.h"

#include <QObject>
#include <QIODevice>

class QIODevice;

namespace Communication {

constexpr uint8_t  SOF_MARKER = 0xA5;
constexpr uint8_t  EOF_MARKER = 0x5A;
constexpr size_t   MIN_PACKET_SIZE = 15;
constexpr size_t   MAX_PACKET_SIZE = 128;
constexpr size_t   HEADER_SYNC_SIZE = 2; // SOF + Length

/*!
 * \class PacketTransceiver
 * \brief Handles binary packet serialization and parsing.
 */
class PacketTransceiver : public QObject {
    Q_OBJECT

  public:
    /*!
     * \brief PacketTransceiver Construcs the packet transeriver
     * \param device A QIO device (QTcpSocket, QSerialport, etc)
     * \param parent QObject parent
     */
    explicit PacketTransceiver(QIODevice* device, QObject* parent = nullptr);

    /*!
     * \brief Calculates a checksum for the given data buffer.
     */
    uint16_t calculate_checksum(const void* data, size_t length);

    /*!
     * \brief Validates a packet's checksum
     * \param packet The raw bytes of the packet to validate.
     * \return True if the packet is valid, false otherwise.
     */
    bool validate_checksum(const QByteArray& packet);

    /*!
     * \brief Retrieves the current epoch time.
     */
    uint32_t get_timestamp();

    void send_command(OPCode cmd);

    template <typename T>
    void send_command(OPCode cmd, const T& payload) {
        transmit(cmd, ++m_global_packet_count, PACKET_TYPE_REQUEST, payload);
    }

    template <typename T>
    void send_reply(OPCode cmd, uint8_t original_req_id, const T& payload) {
        transmit(cmd, original_req_id, PACKET_TYPE_REPLY, payload);
    }

  Q_SIGNALS:
    /*!
     * \brief Emitted when a validated, complete packet is extracted from the stream.
     * \param packet The raw bytes of the validated packet.
     */
    void packet_received(const QByteArray &packet);

  private:
    template <typename T>
    void transmit(OPCode cmd, uint8_t req_id, PacketType type, const T& payload) {
        const uint8_t total_size = sizeof(PacketHeader) + sizeof(OPCode) + sizeof(T) + sizeof(PacketFooter);

        QByteArray buffer;
        buffer.reserve(total_size);

        PacketHeader header;
        header.sof = SOF_MARKER;
        header.packet_length = total_size;
        header.type = static_cast<uint8_t>(type);
        header.request_id = req_id;
        header.timestamp_ms = get_timestamp();
        header.packet_id = ++m_global_packet_count;
        buffer.append(reinterpret_cast<const char*>(&header), sizeof(PacketHeader));

        uint8_t op = static_cast<uint8_t>(cmd);
        buffer.append(reinterpret_cast<const char*>(&op), 1);

        buffer.append(reinterpret_cast<const char*>(&payload), sizeof(T));

        PacketFooter footer;
        footer.checksum = calculate_checksum(buffer.constData(), buffer.size());
        footer.footer = EOF_MARKER;
        buffer.append(reinterpret_cast<const char*>(&footer), sizeof(PacketFooter));

        m_device->write(buffer);
    }

    /*!
     * \brief Internal slot triggered by device's readyRead signal.
     */
    void read_data();

      /*!
     * \brief Parses the the packet.
     */
    void process_buffer();

    uint32_t m_global_packet_count; //!< Number of packets that have been sent.
    uint8_t m_last_request_id;      //!< The ID of the last received request.
    QIODevice* m_device;            //!< The active I/O device.
    QByteArray m_incoming_data;     //!< Buffer for incoming byte .
};
} //namespace Communication

#endif //PACKET_TRANSCEIVER_H