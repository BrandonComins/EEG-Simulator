#include "controlwidget.h"
#include "ui_controlwidget.h"

ControlWidget::ControlWidget(const std::string &channel_id, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ControlWidget)
{
    ui->setupUi(this);

    ui->groupBox_channel->setTitle(QString::fromStdString(channel_id));
}

ControlWidget::~ControlWidget() {
    delete ui;
}

//Bad fix me later. Should be index
auto ControlWidget::channel_id() const -> int {
    auto id = ui->groupBox_channel->title();
    id.remove(QStringLiteral("Channel "));

    return id.toInt();
}

void ControlWidget::set_alpha_frequency_hz(double frequency_hz) {
    ui->doubleSpinBox_alpha_frequency->setValue(frequency_hz);
}

auto ControlWidget::alpha_frequency() const -> double {
    return ui->doubleSpinBox_alpha_frequency->value();
}

void ControlWidget::set_alpha_amplitude_uv(double amplitude_uv) {
    ui->doubleSpinBox_alpha_amplitude->setValue(amplitude_uv);
}

auto ControlWidget::alpha_amplitude_uv() const -> double {
    return ui->doubleSpinBox_alpha_amplitude->value();
}

void ControlWidget::set_beta_frequency_hz(double frequency_hz) {
    ui->doubleSpinBox_beta_frequency->setValue(frequency_hz);
}

auto ControlWidget::beta_frequency_hz() const -> double {
    return ui->doubleSpinBox_beta_frequency->value();
}

void ControlWidget::set_beta_amplitude_uv(double amplitude_uv) {
    ui->doubleSpinBox_beta_amplitude->setValue(amplitude_uv);
}

auto ControlWidget::beta_amplitude_uv() const -> double {
    return ui->doubleSpinBox_beta_amplitude->value();
}

void ControlWidget::set_noise_scale(double scale) {
    ui->doubleSpinBox_noise_scale->setValue(scale);
}

auto ControlWidget::noise_scale() const -> double {
    return ui->doubleSpinBox_noise_scale->value();
}

void ControlWidget::set_noise_persistence(double persistence) {
    ui->doubleSpinBox_noise_persistence->setValue(persistence);
}

auto ControlWidget::noise_persistence() const -> double {
    return ui->doubleSpinBox_noise_persistence->value();
}
