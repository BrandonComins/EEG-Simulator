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
     * \param device A QIO device (Tcp socket, Serial Device, etc)
     * \param parent QObject parent
     */
    explicit PacketTransceiver(QIODevice* device, QObject* parent = nullptr);

    /*!
     * \brief send_command Send a command
     * \param cmd Command Type
     * \param data The payload
     * \param dataSize The size of the paylaod
     */
    void send_command(CommandID cmd, const void* data, size_t data_size);

    /*!
     * \brief Constructs and sends a Reply packet
     * \param cmd The Command being acknowledged.
     * \param data Pointer to the reply payload.
     * \param data_size Size of the reply payload.
     */
    void send_reply(CommandID cmd, const void* data, size_t data_size);

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

  Q_SIGNALS:
    /*!
     * \brief Emitted when a validated, complete packet is extracted from the stream.
     * \param packet The raw bytes of the validated packet.
     */
    void packet_received(const QByteArray &packet);

  private:

    /*!
     * \brief Internal slot triggered by device's readyRead signal.
     */
    void read_data();

      /*!
     * \brief Parses the the packet.
     */
    void process_buffer();

    /*!
     * \brief Sends a packet.
     * \tparam T The packet structure type.
     * \param packet The packet to be sent.
     */
    template <typename T>
    void send_packet(T& packet) {
        if (!m_device || !m_device->isOpen()) return;

        packet.header.sof = SOF_MARKER;
        packet.header.packet_length = static_cast<uint8_t>(sizeof(T));
        packet.footer = EOF_MARKER;

        packet.checksum = calculate_checksum(&packet, sizeof(T));

        m_device->write(reinterpret_cast<const char*>(&packet), sizeof(T));
    }

    /*!
     * \brief Populates the common header and ACK fields for a reply packet.
     * \tparam T The reply packet structure type.
     * \param packet The packet instance to prepare.
     * \param cmd The associated command.
     */
    template <typename T>
    void prepare_reply_header(T& packet, CommandID cmd) {
        packet.header.type = PACKET_TYPE_REPLY;
        packet.header.request_id = m_last_request_id;
        packet.header.packet_id = ++m_global_packet_count;
        packet.header.timestamp_ms = get_timestamp();

        packet.ack.command_id = cmd;
        packet.ack.status_flags = 0;
    }

    uint32_t m_global_packet_count; //!< Number of packets that have been sent.
    uint8_t m_last_request_id;      //!< The ID of the last received request.
    QIODevice* m_device;            //!< The active I/O device.
    QByteArray m_incoming_data;     //!< Buffer for incoming byte .
};
} //namespace Communication

#endif //PACKET_TRANSCEIVER_H