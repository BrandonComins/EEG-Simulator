#include "packets.h"
#include "packettransceiver.h"

#include <QBuffer>
#include <QSignalSpy>
#include <gtest/gtest.h>

class PacketTransceiverTest : public ::testing::Test {
  protected:
    void SetUp() override {
        buffer.open(QIODevice::ReadWrite);
        transceiver = new Communication::PacketTransceiver(&buffer);
    }

    void TearDown() override {
        delete transceiver;
        buffer.close();
    }

    QByteArray buffer_data;
    QBuffer buffer{&buffer_data};
    Communication::PacketTransceiver *transceiver;
};

TEST_F(PacketTransceiverTest, ChecksumCalculationAndValidation) {
    QByteArray pkt = "HelloWorld";
    uint16_t checksum = transceiver->calculate_checksum(pkt.data(), pkt.size());

    pkt.append(reinterpret_cast<const char *>(&checksum), sizeof(uint16_t));
    pkt.append(static_cast<char>(Communication::EOF_MARKER));

    EXPECT_TRUE(transceiver->validate_checksum(pkt));

    pkt[0] = pkt[0] ^ 0xFF;
    EXPECT_FALSE(transceiver->validate_checksum(pkt));
}

TEST_F(PacketTransceiverTest, SendCommandSerialization) {
    Communication::ChannelConstants config;
    config.channel_id = 42;
    config.alpha_frequency = 10.5f;
    config.alpha_amplitude = 1.2f;
    config.noise_persistence = 0.9f;

    buffer_data.clear();
    buffer.seek(0);
    transceiver->send_command(Communication::CMD_SET_CHANNEL_CONSTANTS, config);

    int sof = buffer_data.indexOf(static_cast<char>(Communication::SOF_MARKER));
    ASSERT_NE(sof, -1);

    const auto *header =
        reinterpret_cast<const Communication::PacketHeader *>(buffer_data.data() + sof);
    EXPECT_EQ(header->sof, Communication::SOF_MARKER);

    EXPECT_EQ(header->type, Communication::PACKET_TYPE_REQUEST);

    uint8_t op_code =
        static_cast<uint8_t>(buffer_data.at(sof + sizeof(Communication::PacketHeader)));
    EXPECT_EQ(op_code, Communication::CMD_SET_CHANNEL_CONSTANTS);

    size_t payload_offset = sof + sizeof(Communication::PacketHeader) + sizeof(op_code);
    const auto *captured = reinterpret_cast<const Communication::ChannelConstants *>(
        buffer_data.data() + payload_offset);

    EXPECT_EQ(captured->channel_id, 42);
    EXPECT_FLOAT_EQ(captured->alpha_frequency, 10.5f);
    EXPECT_FLOAT_EQ(captured->alpha_amplitude, 1.2f);
    EXPECT_FLOAT_EQ(captured->noise_persistence, 0.9f);

    uint8_t eof = static_cast<uint8_t>(buffer_data.at(buffer_data.size() - 1));
    EXPECT_EQ(eof, Communication::EOF_MARKER);
}

TEST_F(PacketTransceiverTest, ReceivePacketSignalEmitted) {
    QSignalSpy spy(transceiver, &Communication::PacketTransceiver::packet_received);

    Communication::ChannelConstants config;
    config.channel_id = 99;

    transceiver->send_command(Communication::CMD_SET_CHANNEL_CONSTANTS, config);

    buffer.seek(0);
    Q_EMIT buffer.readyRead();

    EXPECT_EQ(spy.count(), 1);

    QList<QVariant> arguments = spy.takeFirst();
    QByteArray receivedPkt = arguments.front().toByteArray();

    EXPECT_EQ(static_cast<uint8_t>(receivedPkt.at(0)), Communication::SOF_MARKER);
}

TEST_F(PacketTransceiverTest, HandlesGarbageBeforeValidPacket) {
    QSignalSpy spy(transceiver, &Communication::PacketTransceiver::packet_received);

    buffer_data.append(QByteArray::fromHex("FFFFFF001234"));

    Communication::ChannelConstants config;
    transceiver->send_command(Communication::CMD_SET_CHANNEL_CONSTANTS, config);

    buffer.seek(0);
    Q_EMIT buffer.readyRead();

    EXPECT_EQ(spy.count(), 1);
}