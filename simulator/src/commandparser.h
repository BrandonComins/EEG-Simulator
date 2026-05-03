#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "packets.h"

#include <QObject>

class EEG;
class QByteArray;

namespace Communication {

class PacketTransceiver;

/*!
 * \class CommandParser
 * \brief Translates generic requests into hardware actions.
 */
class CommandParser : public QObject {
    Q_OBJECT

  public:
    /*!
     * \brief Constructs the parser with a reference to the simulator instance.
     * \param simulator A reference to the EEGSimulator to be controlled.
     */
    explicit CommandParser(const std::vector<EEG *> &channels, PacketTransceiver *transceiver);
    ~CommandParser() = default;

    /*!
     * \brief process_raw_packet Processes a raw packet
     * \param data The data from the packet
     */
    void process_raw_packet(const QByteArray &data);

    /*!
     * \brief execute_command Does a command based on the packet contents
     * \param cmd The cmd's opcode to execute
     * \param header The header of the packet
     * \param payload The data of the packet
     * \return True if ran a command successfully
     */
    bool execute_command(OPCode cmd, const PacketHeader &header, const char *payload);

  private:
    std::vector<double> m_log_buffer; //!< Info holding samples to be logged
    PacketTransceiver *m_transceiver; //!< Pointer to the packet tranceivers
    std::vector<EEG *> m_channels;    //!< Pointer to the vector holding the channel
};
} // namespace Communication

#endif // COMMAND_PARSER_H