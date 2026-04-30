#include "commandparser.h"
#include "eeg.h"
#include "packettransceiver.h"

#include <QCoreApplication>
#include <QTcpSocket>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    constexpr int connect_time_ms = 1000;
    constexpr int port = 1234;
    const QString host_ip = QStringLiteral("127.0.0.1");
    constexpr int num_channels = 11; //Make Settings File later

    std::vector<EEG> channels(num_channels);
    QTcpSocket socket;
    Communication::PacketTransceiver transceiver(&socket);
    Communication::CommandParser parser(channels, &transceiver);

    auto attempt_connection = [&]() {
        if (socket.state() == QAbstractSocket::UnconnectedState) {
            qDebug() << QStringLiteral("Attempting to connect to Host at (%1, %2)")
                            .arg(host_ip).arg(port);
            socket.connectToHost(host_ip, port);
        }
    };

    QObject::connect(&transceiver, &Communication::PacketTransceiver::packet_received,
                     &parser, &Communication::CommandParser::process_raw_packet);

    QObject::connect(&socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected! Retrying...";
        QTimer::singleShot(connect_time_ms, attempt_connection);
    });

    QObject::connect(&socket, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError) {
        if (socket.state() == QAbstractSocket::UnconnectedState) {
            QTimer::singleShot(connect_time_ms, attempt_connection);
        }
    });

    attempt_connection();

    return app.exec();
}