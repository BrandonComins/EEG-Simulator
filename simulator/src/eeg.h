#ifndef EEG_SIMULATOR_H
#define EEG_SIMULATOR_H

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
    EEG();

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
    void set_noise_persistance(double persistance);

    /*!
     * \brief set_num_channels Set the number of channels in the EEG
     * \param channels The number of channels
     */
    void set_num_channels(int channels);

    /*!
     * \brief num_channels Get the number of channels
     * \return The number of channels
     */
    [[nodiscard]] auto num_channels() -> int;

    /*!
     * \brief Generates a single voltage sample for a given timestamp.
     * \param time_seconds The current simulation time in seconds.
     * \return Calculated voltage in microvolts (uV).
     */
    double get_next_sample(double time_seconds);

private:
    double m_alpha_amplitude_uv; //!< Amplitude of the Alpha wave in uV.
    double m_alpha_freq_hz;      //!< Frequency of the Alpha wave in Hz.
    double m_beta_amplitude_uv;  //!< Amplitude of the Beta wave in uV.
    double m_beta_freq_hz;       //!< Frequency of the Beta wave in Hz.
    double m_noise_scale;        //!< Scaling factor for generated noise.
    double m_noise_persistence;  //!< Coefficient for noise smoothing (0.0 to 1.0).
    double m_last_noise;         /*!< Internal state for the noise filter. */
};

#endif // EEG_SIMULATOR_H