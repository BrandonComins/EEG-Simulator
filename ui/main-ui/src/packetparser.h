#ifndef PACKETPARSER_H
#define PACKETPARSER_H

#include "packets.h"
#include <QObject>

namespace Plot {
class PlotHelper;
class CurveConfig;
}

class PacketParser : public QObject
{
    Q_OBJECT
  public:
    explicit PacketParser(QObject *parent = nullptr);

    /*!
    * \brief Unpacks and routes incoming binary data.
    * \param data The raw, validated binary packet from the transceiver.
    */
    void process_incoming_packet(const QByteArray &data);

  Q_SIGNALS:
    /*!
     * \brief got_channel_data Emiited when the UI receives channel data
     * \param channel_id The name of the channel
     * \param x The X coordinate
     * \param y The Y coordinate
     */
    void got_channel_data(const std::string &channel_id, double x, double y);

    /*!
     * \brief got_new_channel Emmited when a new channel id arrives from the hardware
     * \param channel_id The name of the channel
     * \param config The config for the curve on plot
     */
    void got_new_channel(const std::string &channel_id, const Plot::CurveConfig &config);

  private:
    /*!
     * \brief Extracts a single EEG sample and updates the plot.
     * \param header Pointer to the packet header (for timestamp).
     * \param payload Pointer to the raw EEGSample payload.
     */
    void handle_eeg_data(const Communication::PacketHeader* header, const char* payload);

    /*!
     * \brief Handles the response containing the hardware's channel configuration.
     * \param payload Pointer to the raw ChannelsCount payload.
     */
    void handle_channel_count(const char* payload);

    int m_num_channels; //!< The number of channels in the connected device
};

#endif // PACKETPARSER_H
