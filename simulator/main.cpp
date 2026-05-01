#include "commandparser.h"
#include "eeg.h"
#include "packettransceiver.h"
#include "settings_defs.h"

#include <QSettings>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardPaths>

void synchronize_master_settings() {
    const QString settings_path = SettingsDefs::get_common_settings_path();
    QSettings settings(settings_path, QSettings::IniFormat);

    const QMap<QString, QVariant> defaults = {
        {SettingsDefs::CommonKeys::num_channels,     SettingsDefs::CommonDefaults::num_channels},
        {SettingsDefs::CommonKeys::server_port,      SettingsDefs::CommonDefaults::server_port},
        {SettingsDefs::CommonKeys::server_ip,        SettingsDefs::CommonDefaults::server_ip},
        {SettingsDefs::CommonKeys::connection_retry, SettingsDefs::CommonDefaults::connection_retry}
    };

    bool needs_sync = false;
    for (auto it = defaults.begin(); it != defaults.end(); ++it) {
        if (!settings.contains(it.key())) {
            settings.setValue(it.key(), it.value());
            needs_sync = true;
        }
    }

    if (needs_sync) {
        settings.sync();
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    synchronize_master_settings();

    QSettings common_settings(SettingsDefs::get_common_settings_path(), QSettings::IniFormat);
    const int num_channels       = common_settings.value(SettingsDefs::CommonKeys::num_channels).toInt();
    const int port               = common_settings.value(SettingsDefs::CommonKeys::server_port).toInt();
    const QString host_ip        = common_settings.value(SettingsDefs::CommonKeys::server_ip).toString();
    const int connect_time_ms    = common_settings.value(SettingsDefs::CommonKeys::connection_retry).toInt();

    std::vector<std::unique_ptr<EEG>> channels;
    std::vector<EEG*> channel_observers;

    channels.reserve(num_channels);
    channel_observers.reserve(num_channels);

    for (int i = 0; i < num_channels; ++i) {
        channels.push_back(std::make_unique<EEG>(i));
        channel_observers.push_back(channels.back().get());
    }

    QTcpSocket socket;
    Communication::PacketTransceiver transceiver(&socket);
    Communication::CommandParser parser(channel_observers, &transceiver);

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