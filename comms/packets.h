#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>

namespace Communication {

#pragma pack(push, 1)

enum PacketType {
    PACKET_TYPE_REQUEST, // UI -> Hardware
    PACKET_TYPE_REPLY    // Hardware -> UI
};

//Command OP Codes. IMPORTANT: Only add new commands to the end of the enum to not break backwards compatability between versions!!!!
enum OPCode : uint8_t {
    CMD_GET_LATEST_DATA,
    CMD_GET_NUM_CHANNELS,
    CMD_SET_ALPHA_AMPLITUDE,
    CMD_SET_ALPHA_FREQUENCY,
    CMD_SET_BETA_AMPLITUDE,
    CMD_SET_BETA_FREQUENCY,
    CMD_SET_NOISE_SCALE,
    CMD_SET_NOISE_PERSISTANCE,
};

//!< ----GENERIC----

struct PacketHeader {
    uint8_t  sof;             //!< Start of Frame (0xA5)
    uint8_t  packet_length;   //!< Total size of the packet
    uint8_t  type;            //!< PacketType
    uint8_t  request_id;      //!< Unique ID to pair Requests with Replies
    uint32_t packet_id;       //!< Global sequence counter
    uint32_t timestamp_ms;    //!< Device/System time
}; // 12 Bytes

struct PacketFooter {
    uint16_t checksum;
    uint8_t  footer;
}; // 3 Bytes

struct GenericAck {
    uint8_t  command_id;     //!< Which command this reply belongs to
    uint16_t status;         //!< System health (0 = OK)
}; // 3 Bytes

//!< ----SET PACKETS----

struct Amplitude {
    float amplitude;         //!< Amplitude of the alpha config
    uint8_t channel_id;      //!< Channel ID to update
}; // 4 Bytes

struct Frequency {
    float frequency;         //!< Frequency of the alpha config
    uint8_t channel_id;      //!< Channel ID to update
}; // 4 Bytes

struct NoiseScale {
    float scale;             //!< Noise Scale
    uint8_t channel_id;      //!< Channel ID to update
}; // 4 Bytes

struct NoisePersistance {
    float persistance;       //!< Noise Persistance
    uint8_t channel_id;      //!< Channel ID to update
}; // 5 Bytes

struct ChannelsCount {
    uint8_t num_channels;    //!< Actual data
}; // 1 Byte

//!< ----GET PACKETS----

struct RequestSample {
    uint8_t channel_id;       //!< The channel index being requested
}; // 1 Byte

struct EEGSample {
    float    value;     //!< Value from the sample
    uint8_t  quality;   //!< Quality of the sample
    uint8_t channel_id; //!< Channel the data is coming from
}; // 6 Bytes

#pragma pack(pop)

} //namespace Communication

#endif // PACKETS_H