#include "commandparser.h"
#include "eeg.h"
#include "fmt/base.h"
#include "packettransceiver.h"
#include "settings_defs.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <spdlog/spdlog.h>
#include <QSettings>
#include <QCoreApplication>
#include <QTcpSocket>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>

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

void init_logging(int num_channels) {
    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString log_dir = path + "/EEG/simulator/logs";

    if (!QDir(log_dir).exists()) {
        QDir().mkpath(log_dir);
    }

    constexpr int queue_size = 8192;
    constexpr int worker_threads = 1;
    spdlog::init_thread_pool(queue_size, worker_threads);

    const bool clear_on_start = true;
    auto log_path = (log_dir + "/channels.log").toStdString();
    auto async_file = spdlog::basic_logger_mt<spdlog::async_factory>(
        "channel_logger",
        log_path,
        clear_on_start
        );

    spdlog::set_default_logger(async_file);

    spdlog::set_pattern("%v");

    std::string keys = "Time";
    for(int id = 0; id < num_channels; ++id) {
        keys+=fmt::format(", CH_{}", id);
    }

    constexpr int time_seconds = 3;
    spdlog::info(keys);
    spdlog::flush_every(std::chrono::seconds(time_seconds));

    fmt::println("Logging initialized at: {}", log_path);
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

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
            fmt::println("Attempting to connect to Host at ({}, {})", host_ip.toStdString(), port);
            socket.connectToHost(host_ip, port);
        }
    };

    init_logging(num_channels);

    QObject::connect(&transceiver, &Communication::PacketTransceiver::packet_received,
                     &parser, &Communication::CommandParser::process_raw_packet);

    QObject::connect(&socket, &QTcpSocket::disconnected, [&]() {
        fmt::println("Disconnected! Retrying...");
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