#include "eeg.h"
#include "packets.h"

#include <cmath>

constexpr double default_alpha_amplitude_uv = 20.0;
constexpr double default_alpha_freq_hz      = 10.0;
constexpr double default_beta_amplitude_uv  = 10.0;
constexpr double default_beta_freq_hz       = 20.0;
constexpr double default_noise_scale        = 5.0;
constexpr double default_noise_persistence  = 0.9;

EEG::EEG()
    : m_alpha_amplitude_uv(default_alpha_amplitude_uv)
    , m_alpha_freq_hz(default_alpha_freq_hz)
    , m_beta_amplitude_uv(default_beta_amplitude_uv)
    , m_beta_freq_hz(default_beta_freq_hz)
    , m_noise_scale(default_noise_scale)
    , m_noise_persistence(default_noise_persistence)
    , m_last_noise(0.0) {/*empty*/ }

double EEG::get_next_sample(double time_seconds) {
    // Wave Equation: y(t) = A * sin(2 * PI * f * t)
    double alpha_wave = m_alpha_amplitude_uv * std::sin(2.0 * M_PI * m_alpha_freq_hz * time_seconds);
    double beta_wave  = m_beta_amplitude_uv  * std::sin(2.0 * M_PI * m_beta_freq_hz * time_seconds);

    // Generate white noise in range [-0.5, 0.5] * scale
    double white_noise = ((static_cast<double>(rand()) / RAND_MAX) - 0.5) * m_noise_scale;

    // Recursive Low-Pass Filter (Pink Noise simulation)
    m_last_noise = (m_noise_persistence * m_last_noise) + white_noise;

    return alpha_wave + beta_wave + m_last_noise;
}

void EEG::EEG::fill_latest_samples(Communication::EEGSample *packet_data, size_t num_channels, double time_seconds) {
    for (size_t channel = 0; channel < num_channels; ++channel) {
        double channel_specific_time = time_seconds + (static_cast<double>(channel) * 0.01);
        double val = get_next_sample(channel_specific_time);
        double local_jitter = ((static_cast<double>(rand()) / RAND_MAX) - 0.5) * (m_noise_scale * 0.2);

        packet_data[channel].value = static_cast<float>(val + local_jitter);
        packet_data[channel].quality = 100;
    }
}

void EEG::set_alpha_params(double amplitude, double frequency) {
    m_alpha_amplitude_uv = amplitude;
    m_alpha_freq_hz = frequency;
}

void EEG::set_beta_params(double amplitude, double frequency) {
    m_beta_amplitude_uv = amplitude;
    m_beta_freq_hz = frequency;
}

void EEG::set_noise_level(double scale) {
    m_noise_scale = scale;
}
