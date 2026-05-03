#ifndef EEG_SIMULATOR_H
#define EEG_SIMULATOR_H

#include <memory>
#include <random>

class QSettings;

namespace Communication {
struct EEGSample;
}

/*!
 * \class EEG
 * \brief A high-performance, this class generates EEG data
 */
class EEG {
  public:
    /*!
     * \brief Constructs the simulator.
     */
    EEG(int id);

    ~EEG();

    /*!
     * \brief Updates the amplitude for the Alpha rhythm.
     * \param amplitude The target amplitude in microvolts (uV).
     */
    void set_alpha_amplitude(double amplitude_uV);

    /*!
     * \brief Updates the frequency for the Alpha rhythm.
     * \param frequency The target frequency in Hertz (Hz).
     */
    void set_alpha_frequency(double frequency_Hz);

    /*!
     * \brief Updates the amplitude for the Beta rhythm.
     * \param amplitude The target amplitude in microvolts (uV).
     */
    void set_beta_amplitude(double amplitude_uV);

    /*!
     * \brief Updates the frequency for the Beta rhythm.
     * \param frequency The target frequency in Hertz (Hz).
     */
    void set_beta_frequency(double frequency_Hz);

    /*!
     * \brief Sets the intensity of the background noise.
     * \param scale The scaling factor applied to the generated noise.
     */
    void set_noise_level(double scale_uV);

    /*!
     * \brief Sets the noise smoothing coefficient.
     * \param persistence A ratio (0.0 to 1.0) determining the "pinkness" of the noise.
     */
    void set_noise_persistence(double persistence);

    /*!
     * \brief Generates a single voltage sample for a given timestamp.
     * \param time_seconds The current simulation time in seconds.
     * \return Calculated voltage in microvolts (uV).
     */
    [[nodiscard]] auto get_next_sample(double time_seconds) -> double;

    /*!
     * \brief alpha_amplitude_uv Get the alpha amplitude in microvolts (uV)
     * \return the alpha almplitude in microvolts (uV)
     */
    [[nodiscard]] auto alpha_amplitude_uv() const -> double;

    /*!
     * \brief alpha_freq_hz get the alpha frequency in Herts (Hz)
     * \return the alpha frequency in Herts (Hz)
     */
    [[nodiscard]] auto alpha_freq_hz() const -> double;

    /*!
     * \brief beta_amplitude_uv Get the beta amplitude in microvolts (uV)
     * \return the beta almplitude in microvolts (uV)
     */
    [[nodiscard]] auto beta_amplitude_uv() const -> double;

    /*!
     * \brief beta_freq_hz get the beta frequency in Herts (Hz)
     * \return the beta frequency in Herts (Hz)
     */
    [[nodiscard]] auto beta_freq_hz() const -> double;

    /*!
     * \brief noise_level Get the intensity of the background noise
     * \return The noise
     */
    [[nodiscard]] auto noise_level() const -> double;

    /*!
     * \brief noise_persistence Get the noise smoothing coefficient
     * \return A ratio (0.0 to 1.0) determining the "pinkness" of the noise.
     */
    [[nodiscard]] auto noise_persistence() const -> double;

    void initialize_from_settings();

  private:
    int m_id;                              //!< Unique ID of the eeg.
    std::mt19937 m_seed;                   //!< Seed for random variation
    double m_alpha_amplitude_uv;           //!< Amplitude of the Alpha wave in uV.
    double m_alpha_freq_hz;                //!< Frequency of the Alpha wave in Hz.
    double m_beta_amplitude_uv;            //!< Amplitude of the Beta wave in uV.
    double m_beta_freq_hz;                 //!< Frequency of the Beta wave in Hz.
    double m_noise_scale;                  //!< Scaling factor for generated noise.
    double m_noise_persistence;            //!< Coefficient for noise smoothing (0.0 to 1.0).
    double m_last_noise;                   //!< Internal state for the noise filter.
    std::unique_ptr<QSettings> m_settings; //!< Settings file containing constants
};

#endif // EEG_SIMULATOR_H