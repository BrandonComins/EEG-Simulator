#ifndef SETTINGS_DEFS_H
#define SETTINGS_DEFS_H

#include <QString>
#include <QStandardPaths>
#include <QDir>

namespace SettingsDefs {

    inline QString get_config_dir() {
        QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                     + QStringLiteral("/EEG/simulator/settings");
        QDir().mkpath(path);
        return path;
    }

    inline QString get_common_settings_path() {
        return get_config_dir() + QStringLiteral("/common.ini");
    }

    inline QString get_channel_settings_path(int id) {
        return get_config_dir() + QStringLiteral("/channel-%1.ini").arg(id);
    }

    //!< ------ commoni.ini ------
    namespace CommonKeys {
        const QString num_channels      = QStringLiteral("hardware/number_of_channels");
        const QString server_port       = QStringLiteral("network/port");
        const QString server_ip         = QStringLiteral("network/ip");
        const QString connection_retry  = QStringLiteral("network/retry_ms");
    }

    namespace CommonDefaults {
        constexpr int num_channels      = 11;
        constexpr int server_port       = 1234;
        constexpr int connection_retry  = 1000;
        const QString server_ip         = QStringLiteral("127.0.0.1");
    }

    //!< ------channel-X.ini ------
    namespace ChannelKeys {
        const QString alpha_freq   = QStringLiteral("constants/alpha_frequency_hz");
        const QString alpha_amp    = QStringLiteral("constants/alpha_amplitude_uv");
        const QString beta_freq    = QStringLiteral("constants/beta_frequency_hz");
        const QString beta_amp     = QStringLiteral("constants/beta_amplitude_uv");
        const QString noise_scale  = QStringLiteral("constants/noise_scale");
        const QString persistence  = QStringLiteral("constants/noise_persistence");
    }

    namespace ChannelDefaults {
        constexpr double alpha_amplitude   = 10.0;
        constexpr double beta_amplitde    = 5.0;
        constexpr double noise_scale = 0.05;
        constexpr double persistence = 0.9;

        constexpr double frontal_alpha_freq = 8.5;
        constexpr double frontal_beta_freq  = 22.0;
        constexpr double occipital_alpha_freq = 10.0;
        constexpr double occipital_beta_freq  = 15.0;
        constexpr double central_alpha_freq = 9.5;
        constexpr double central_beta_freq  = 18.0;
    }
}

#endif
