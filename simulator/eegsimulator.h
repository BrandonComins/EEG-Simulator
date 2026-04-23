#ifndef EEG_SIMULATOR_H
#define EEG_SIMULATOR_H

/*!
 * \class EEGSimulator
 * \brief A high-performance, pure C++ class for simulating multi-rhythm EEG signals.
 *
 * This class generates synthetic EEG data by combining Alpha and Beta brain rhythms
 * with a filtered noise model to simulate realistic biological signals.
 */
class EEGSimulator {
public:

    /*!
     * \brief Constructs the simulator.
     */
    EEGSimulator();

    /*!
     * \brief Generates a single voltage sample for a given timestamp.
     * \param time_seconds The current simulation time in seconds.
     * \return Calculated voltage in microvolts (uV).
     */
    double get_next_sample(double time_seconds);

    /*!
     * \brief Updates the parameters for the Alpha rhythm.
     * \param amplitude The target amplitude in microvolts (uV).
     * \param frequency The target frequency in Hertz (Hz).
     */
    void set_alpha_params(double amplitude, double frequency);

    /*!
     * \brief Updates the parameters for the Beta rhythm.
     * \param amplitude The target amplitude in microvolts (uV).
     * \param frequency The target frequency in Hertz (Hz).
     */
    void set_beta_params(double amplitude, double frequency);

    /*!
     * \brief Sets the intensity of the background noise.
     * \param scale The scaling factor applied to the generated noise.
     */
    void set_noise_level(double scale);

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