#include "eegsimulator.h"

#include <cmath>

constexpr double default_alpha_amplitude_uv = 20.0;
constexpr double default_alpha_freq_hz      = 10.0;
constexpr double default_beta_amplitude_uv  = 10.0;
constexpr double default_beta_freq_hz       = 20.0;
constexpr double default_noise_scale        = 5.0;
constexpr double default_noise_persistence  = 0.9;

EEGSimulator::EEGSimulator()
    : m_alpha_amplitude_uv(default_alpha_amplitude_uv)
    , m_alpha_freq_hz(default_alpha_freq_hz)
    , m_beta_amplitude_uv(default_beta_amplitude_uv)
    , m_beta_freq_hz(default_beta_freq_hz)
    , m_noise_scale(default_noise_scale)
    , m_noise_persistence(default_noise_persistence)
    , m_last_noise(0.0) {/*empty*/ }

double EEGSimulator::get_next_sample(double time_seconds) {
    // Wave Equation: y(t) = A * sin(2 * PI * f * t)
    double alpha_wave = m_alpha_amplitude_uv * std::sin(2.0 * M_PI * m_alpha_freq_hz * time_seconds);
    double beta_wave  = m_beta_amplitude_uv  * std::sin(2.0 * M_PI * m_beta_freq_hz * time_seconds);

    // Generate white noise in range [-0.5, 0.5] * scale
    double white_noise = ((static_cast<double>(rand()) / RAND_MAX) - 0.5) * m_noise_scale;

    // Recursive Low-Pass Filter (Pink Noise simulation)
    m_last_noise = (m_noise_persistence * m_last_noise) + white_noise;

    return alpha_wave + beta_wave + m_last_noise;
}

void EEGSimulator::set_alpha_params(double amplitude, double frequency) {
    m_alpha_amplitude_uv = amplitude;
    m_alpha_freq_hz = frequency;
}

void EEGSimulator::set_beta_params(double amplitude, double frequency) {
    m_beta_amplitude_uv = amplitude;
    m_beta_freq_hz = frequency;
}

void EEGSimulator::set_noise_level(double scale) {
    m_noise_scale = scale;
}
