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
enum CommandID {
    CMD_GET_LATEST_DATA,
    CMD_SET_ALPHA_PARAMS,
    CMD_SET_BETA_PARAMS,
    CMD_SET_NOISE_LEVEL
};

struct PacketHeader {
    uint8_t  sof;             //!< Start of Frame (0xA5)
    uint8_t  packet_length;   //!< Total size of the packet
    uint8_t  type;            //!< PacketType
    uint8_t  request_id;      //!< Unique ID to pair Requests with Replies
    uint32_t packet_id;       //!< Global sequence counter
    uint32_t timestamp_ms;    //!< Device/System time
}; // 12 Bytes

struct GenericAckReply {
    uint8_t  command_id;      //!< Which command this reply belongs to
    uint16_t status_flags;    //!< System health (0 = OK)
}; // 3 Bytes

struct AlphaConfig {
    float amplitude;          //!< Amplitude of the alpha config
    float frequency;          //!< Frequency of the alpha config
}; // 8 Bytes

struct BetaConfig {
    float amplitude;          //!< Amplitude of the beta config
    float frequency;          //!< Frequency of the beta config
}; // 8 Bytes

struct NoiseConfig {
    float scale;             //!< Noise Scale
}; // 4 Bytes

struct GenericCommandRequest {
    PacketHeader header;      //!< Header metadata
    uint8_t      command_id;  //!< Command
    uint8_t      payload[16]; //!< Actual data
    uint16_t     checksum;    //!< Checksum to verify a valid packet
    uint8_t      footer;      //!< End of the packet
}; // 31 Bytes (For Now)

struct EEGSample {
    float    value;   //!< Value from the sample
    uint8_t  quality; //!< Quality of the sample
}; // 5 Bytes

struct EEGDataReply {
    PacketHeader    header;       //!< Header metadata
    GenericAckReply ack;          //!< Acknowledgement packet
    EEGSample       channels[11]; //!< Sample data for 11 channels
    uint16_t        checksum;     //!< Checksum to verify a valid packet
    uint8_t         footer;       //!< End of the packet
}; // Total: 72 Bytes

struct GenericStatusReply {
    PacketHeader    header;    //!< Header metadata
    GenericAckReply ack;       //!< Acknowledgement packet
    uint16_t        checksum;  //!< Checksum to verify a valid packet
    uint8_t         footer;    //!< End of the packet
}; // Total: 17 Bytes

#pragma pack(pop)

} //namespace Communication

#endif // PACKETS_H