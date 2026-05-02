#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget {
    Q_OBJECT

  public:
    explicit ControlWidget(const std::string &channel_id, QWidget *parent = nullptr);
    ~ControlWidget();

    /*!
     * \brief Gets the unique identifier for this channel.
     * \return The id of the channel.
     */
    [[nodiscard]] auto channel_id() const -> int;

    /*!
     * \brief Sets the frequency_hz of the Alpha band component.
     * \param frequency_hz The frequency_hz in Hertz (Hz).
     */
    void set_alpha_frequency_hz(double frequency_hz);

    /*!
     * \brief Gets the current frequency of the Alpha band component.
     * \return The frequency in Hertz (Hz).
     */
    [[nodiscard]] auto alpha_frequency() const -> double;

    /*!
     * \brief Sets the amplitude_uv of the Alpha band component.
     * \param amplitude_uv The amplitude_uv in microvolts (uV).
     */
    void set_alpha_amplitude_uv(double amplitude_uv);

    /*!
     * \brief Gets the current amplitude of the Alpha band component.
     * \return The amplitude in microvolts (uV).
     */
    [[nodiscard]] auto alpha_amplitude_uv() const -> double;

    /*!
     * \brief Sets the frequency_hz of the Beta band component.
     * \param frequency_hz The frequency_hz in Hertz (Hz).
     */
    void set_beta_frequency_hz(double frequency_hz);

    /*!
     * \brief Gets the current frequency of the Beta band component.
     * \return The frequency in Hertz (Hz).
     */
    [[nodiscard]] auto beta_frequency_hz() const -> double;

    /*!
     * \brief Sets the amplitude_uv of the Beta band component.
     * \param amplitude_uv The amplitude_uv in microvolts (uV).
     */
    void set_beta_amplitude_uv(double amplitude_uv);

    /*!
     * \brief Gets the current amplitude of the Beta band component.
     * \return The amplitude in microvolts (uV).
     */
    [[nodiscard]] auto beta_amplitude_uv() const -> double;

    /*!
     * \brief Sets the scaling factor for the signal noise.
     * \param scale A multiplier for the noise magnitude.
     */
    void set_noise_scale(double scale);

    /*!
     * \brief Gets the current scaling factor for the signal noise.
     * \return The noise scale multiplier.
     */
    [[nodiscard]] auto noise_scale() const -> double;

    /*!
     * \brief Sets the persistence (fractal roughness) of the noise.
     * \param persistence A value typically between 0.0 and 1.0.
     */
    void set_noise_persistence(double persistence);

    /*!
     * \brief Gets the current noise persistence value.
     * \return The persistence factor.
     */
    [[nodiscard]] auto noise_persistence() const -> double;

  private:
    Ui::ControlWidget *ui;
};

#endif // CONTROLWIDGET_H
