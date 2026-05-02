#include "fmt/base.h"
#include "packettransceiver.h"

#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <chrono>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QByteArray buffer;
    QBuffer device(&buffer);
    device.open(QIODevice::ReadWrite);

    Communication::PacketTransceiver transceiver(&device);

    QObject::connect(
        &transceiver, &Communication::PacketTransceiver::packet_received,
        [&](const QByteArray &packet) {
            fmt::println("--- New Packet Received ---");
            fmt::println("Total Bytes: {}", packet.size());

            const auto *header =
                reinterpret_cast<const Communication::PacketHeader *>(packet.constData());
            fmt::println("Packet ID: {}", header->packet_id);
            fmt::println("Request ID: {}", header->request_id);

            const char *payload = packet.constData() + sizeof(Communication::PacketHeader) + 1;
            auto *constants = reinterpret_cast<const Communication::ChannelConstants *>(payload);

            fmt::println("Payload Channel ID: {}", constants->channel_id);
            fmt::println("Alpha Freq: {}", constants->alpha_frequency);
        });

    Communication::ChannelConstants config;
    config.channel_id = 5;
    config.alpha_frequency = 10.5f;
    config.alpha_amplitude = 2.0f;
    config.beta_frequency = 22.0f;
    config.beta_amplitude = 1.5f;
    config.noise_level = 0.01f;
    config.noise_persistence = 0.5f;

    fmt::println("Sending CMD_SET_CHANNEL_CONSTANTS...\r\n");

    auto t1 = std::chrono::high_resolution_clock::now();

    transceiver.send_command(Communication::CMD_SET_CHANNEL_CONSTANTS, config);

    auto t2 = std::chrono::high_resolution_clock::now();

    device.seek(0);

    auto t3 = std::chrono::high_resolution_clock::now();

    Q_EMIT device.readyRead();

    auto t4 = std::chrono::high_resolution_clock::now();

    auto d_send = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto d_signal = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    auto d_parsing = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
    auto d_total = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t1).count();

    fmt::println("--------------------------");
    fmt::println("Send    : {:>6} us", d_send);
    fmt::println("Signal  : {:>6} us", d_signal);
    fmt::println("Parsing : {:>6} us", d_parsing);
    fmt::println("Total   : {:>6} us", d_total);
    fmt::println("--------------------------");

    return 0;
}
