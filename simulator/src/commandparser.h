#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <QObject>

class EEG;
class QByteArray;

namespace Communication {

class PacketTransceiver;
struct GenericCommandRequest;

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
      explicit CommandParser(EEG *simulator, PacketTransceiver *transceiver);

    void process_raw_packet(const QByteArray &data);

    /*!
     * \brief Interprets a request and executes the corresponding simulator call.
     * \param request The incoming packet.
     * \return true if the command was successfully parsed and executed
     */
    bool process_request(const GenericCommandRequest& request);

  private:
    PacketTransceiver *m_transceiver; //!< Pointer to the packet tranceiver
    EEG *m_eeg;                       //!< Pointer to the EEG sim
};
} //namespace Communication

#endif // COMMAND_PARSER_H