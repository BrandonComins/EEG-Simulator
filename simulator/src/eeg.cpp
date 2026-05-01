#include "eeg.h"
#include "settings_defs.h"
#include "spdlog/spdlog.h"

#include <algorithm>
#include <cmath>
#include <QSettings>
#include <random>

EEG::EEG(int id)
    : m_id(id)
    , m_seed(static_cast<unsigned int>(
          std::chrono::high_resolution_clock::now().time_since_epoch().count() + id))
    , m_last_noise(0.0)
    , m_settings(std::make_unique<QSettings>(
          SettingsDefs::get_channel_settings_path(id),
          QSettings::IniFormat))
{

    initialize_from_settings();
}

EEG::~EEG() {
    m_settings->sync();
}

double get_random_dist(std::mt19937 &seed){
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    return dist(seed);
}

double EEG::get_next_sample(double time_seconds) {
    // Wave Equation: y(t) = A * sin(2 * PI * f * t)
    double alpha_wave = m_alpha_amplitude_uv * std::sin(2.0 * M_PI * m_alpha_freq_hz * time_seconds);
    double beta_wave  = m_beta_amplitude_uv  * std::sin(2.0 * M_PI * m_beta_freq_hz * time_seconds);

    // Generate white noise in range [-0.5, 0.5] * scale
    double white_noise = get_random_dist(m_seed) * m_noise_scale;

    // Recursive Low-Pass Filter (Pink Noise simulation)
    m_last_noise = (m_noise_persistence * m_last_noise) + white_noise;

    return alpha_wave + beta_wave + m_last_noise;
}

double EEG::alpha_amplitude_uv() const {
    return m_alpha_amplitude_uv;
}

double EEG::alpha_freq_hz() const {
    return m_alpha_freq_hz;
}

double EEG::beta_amplitude_uv() const {
    return m_beta_amplitude_uv;
}

double EEG::beta_freq_hz() const {
    return m_beta_freq_hz;
}

double EEG::noise_level() const {
    return m_noise_scale;
}

double EEG::noise_persistence() const {
    return m_noise_persistence;
}

void EEG::set_alpha_amplitude(double amplitude) {
    m_alpha_amplitude_uv = amplitude;
    m_settings->setValue(SettingsDefs::ChannelKeys::alpha_amp, amplitude);
}

void EEG::set_alpha_frequency(double frequency) {
    m_alpha_freq_hz = frequency;
    m_settings->setValue(SettingsDefs::ChannelKeys::alpha_freq, frequency);
}

void EEG::set_beta_amplitude(double amplitude) {
    m_beta_amplitude_uv = amplitude;
    m_settings->setValue(SettingsDefs::ChannelKeys::beta_amp, amplitude);
}

void EEG::set_beta_frequency(double frequency) {
    m_beta_freq_hz = frequency;
    m_settings->setValue(SettingsDefs::ChannelKeys::beta_freq, frequency);
}

void EEG::set_noise_level(double scale) {
    m_noise_scale = scale;
    m_settings->setValue(SettingsDefs::ChannelKeys::noise_scale, scale);
}

void EEG::set_noise_persistence(double persistence) {
    auto min = 0.0;
    auto max = 1.0;

    m_noise_persistence = std::clamp(persistence, min, max);
    m_settings->setValue(SettingsDefs::ChannelKeys::persistence, m_noise_persistence);
}

void EEG::initialize_from_settings() {
    double default_alpha_f = SettingsDefs::ChannelDefaults::central_alpha_freq;
    double default_beta_f  = SettingsDefs::ChannelDefaults::central_beta_freq;

    if (m_id <= 3) {
        default_alpha_f = SettingsDefs::ChannelDefaults::frontal_alpha_freq;
        default_beta_f  = SettingsDefs::ChannelDefaults::frontal_beta_freq;
    } else if (m_id >= 8) {
        default_alpha_f = SettingsDefs::ChannelDefaults::occipital_alpha_freq;
        default_beta_f  = SettingsDefs::ChannelDefaults::occipital_beta_freq;
    }

    if (!m_settings->contains(SettingsDefs::ChannelKeys::alpha_freq)) {
        double unique_alpha = default_alpha_f + (get_random_dist(m_seed) * 0.5);
        double unique_beta  = default_beta_f  + (get_random_dist(m_seed) * 1.0);

        double amp_modifier = 1.0 + (get_random_dist(m_seed));
        double unique_alpha_a = SettingsDefs::ChannelDefaults::alpha_amplitude * amp_modifier;
        double unique_beta_a  = SettingsDefs::ChannelDefaults::beta_amplitde * (2.0 - amp_modifier);

        m_settings->setValue(SettingsDefs::ChannelKeys::alpha_freq, unique_alpha);
        m_settings->setValue(SettingsDefs::ChannelKeys::beta_freq,  unique_beta);
        m_settings->setValue(SettingsDefs::ChannelKeys::alpha_amp, unique_alpha_a);
        m_settings->setValue(SettingsDefs::ChannelKeys::beta_amp,  unique_beta_a);
        m_settings->setValue(SettingsDefs::ChannelKeys::noise_scale, SettingsDefs::ChannelDefaults::noise_scale);
        m_settings->setValue(SettingsDefs::ChannelKeys::persistence, SettingsDefs::ChannelDefaults::persistence);

        m_settings->sync();
    }

    m_alpha_freq_hz      = m_settings->value(SettingsDefs::ChannelKeys::alpha_freq).toDouble();
    m_alpha_amplitude_uv = m_settings->value(SettingsDefs::ChannelKeys::alpha_amp).toDouble();
    m_beta_freq_hz       = m_settings->value(SettingsDefs::ChannelKeys::beta_freq).toDouble();
    m_beta_amplitude_uv  = m_settings->value(SettingsDefs::ChannelKeys::beta_amp).toDouble();
    m_noise_scale        = m_settings->value(SettingsDefs::ChannelKeys::noise_scale).toDouble();
    m_noise_persistence  = m_settings->value(SettingsDefs::ChannelKeys::persistence).toDouble();
}


