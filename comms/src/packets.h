#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>

namespace Communication {

enum PacketType {
    PACKET_TYPE_REQUEST, //!< UI -> Hardware
    PACKET_TYPE_REPLY    //!< Hardware -> UI
};

/* IMPORTANT: Only add new commands to the end of the enum to
 *  not break backwards compatability between versions!!!!
 */
enum OPCode : uint8_t {
    CMD_GET_LATEST_DATA,
    CMD_GET_NUM_CHANNELS,
    CMD_GET_CHANNEL_CONSTANTS,
    CMD_SET_CHANNEL_CONSTANTS,
};

#pragma pack(push, 1)

//!< ----GENERIC----

struct PacketHeader {
    uint8_t sof;           //!< Start of Frame (0xA5)
    uint8_t packet_length; //!< Total size of the packet
    uint8_t type;          //!< PacketType
    uint8_t request_id;    //!< Unique ID to pair Requests with Replies
    uint32_t packet_id;    //!< Global sequence counter
    uint32_t timestamp_ms; //!< Device/System time
}; // 12 Bytes

struct PacketFooter {
    uint16_t checksum;
    uint8_t footer;
}; // 3 Bytes

struct GenericAck {
    uint8_t command_id; //!< Which command this reply belongs to
    uint16_t status;    //!< System health (0 = OK)
}; // 3 Bytes

//!< ----SET PACKETS----

struct ChannelsCount {
    uint8_t num_channels; //!< Actual data
}; // 1 Byte

//!< ----GET PACKETS----

struct RequestChannelConstants {
    uint8_t channel_id; //!< The channel index being requested
}; // 1 Byte

struct RequestSample {
    uint8_t channel_id; //!< The channel index being requested
}; // 1 Byte

struct EEGSample {
    float value;        //!< Value from the sample
    uint8_t quality;    //!< Quality of the sample
    uint8_t channel_id; //!< Channel the data is coming from
}; // 6 Bytes

//!< ---- SET & GET PACKETS ----

struct ChannelConstants {
    float alpha_frequency;   //!< Frequency of the alpha config
    float alpha_amplitude;   //!< Amplitude of the alpha config
    float beta_frequency;    //!< Frequency of the beta config
    float beta_amplitude;    //!< Amplitude of the beta config
    float noise_level;       //!< Noise Scale
    float noise_persistence; //!< Noise Persistence
    uint8_t channel_id;      //!< Channel ID to update
}; // 25 Bytes

#pragma pack(pop)

} // namespace Communication

#endif // PACKETS_H