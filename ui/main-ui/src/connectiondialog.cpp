#include "connectiondialog.h"

#include "plothelper.h"
#include "ui_connectiondialog.h"

#include <QSettings>
#include <qtimer.h>

constexpr int default_port = 1234;

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , m_server_on(false)
    , m_last_x_axis_point(0.0)
    , m_bytes_received(0)
    , m_stats_timer(new QTimer(this))
    , m_settings(new QSettings(QStringLiteral("EEG"), QStringLiteral("Ui Connection"), this))
    , ui(new Ui::ConnectionDialog) {
    ui->setupUi(this);

    m_plot = new Plot::PlotHelper(ui->frame, this);
    m_plot->set_view_mode(Plot::ROLLING);
    m_plot->set_axis_titles("Seconds Since Server Start", "KB/s");
    m_plot->set_scale_x(0, 10);
    m_plot->set_scale_y(0, 15);

    setWindowTitle(QStringLiteral("Connection Dialog"));

    set_led_status(false);

    int saved_port = m_settings->value("last_used_port", default_port).toInt();
    ui->spinBox_port->setValue(saved_port);

    QObject::connect(ui->pushButton_connect, &QPushButton::clicked, this,
                     &ConnectionDialog::toggle_server);

    QObject::connect(m_stats_timer, &QTimer::timeout, this, &ConnectionDialog::calculate_speed);
}

ConnectionDialog::~ConnectionDialog() {
    delete ui;
}

void ConnectionDialog::user_connected() {
    set_led_status(true);
}

void ConnectionDialog::user_disconnected() {
    set_led_status(false);
}

void ConnectionDialog::toggle_server() {
    if (!m_server_on) {
        constexpr int time_ms = 1000;
        m_server_on = true;
        ui->pushButton_connect->setText(QStringLiteral("Stop Server"));
        m_stats_timer->start(time_ms);
        Q_EMIT start_server_requested(ui->spinBox_port->value());

        // Save the last used port
        m_settings->setValue("last_used_port", ui->spinBox_port->value());
        m_settings->sync();
    } else {
        m_server_on = false;
        set_led_status(false);
        ui->pushButton_connect->setText(QStringLiteral("Start Server"));
        m_stats_timer->stop();
        m_bytes_received = 0;
        Q_EMIT stop_server_requested();
    }
}

void ConnectionDialog::calculate_speed() {
    constexpr double b_to_kb = 1024.0;
    double speed_kbps = static_cast<double>(m_bytes_received) / b_to_kb;

    m_bytes_received = 0;
    m_plot->add_point("Speed", m_last_x_axis_point++, speed_kbps);
}

void ConnectionDialog::ConnectionDialog::set_led_status(bool connected) {
    if (connected) {
        ui->label_status_led->setStyleSheet("background-color: #2ecc71; "
                                            "border-radius: 10px; "
                                            "border: 2px solid #2ecc71; "
                                            "min-width: 20px; "
                                            "min-height: 20px;");
    } else {
        ui->label_status_led->setStyleSheet("background-color: #c0392b; "
                                            "border-radius: 10px; "
                                            "border: 2px solid #c0392b; "
                                            "min-width: 20px; "
                                            "min-height: 20px;");
    }
}

void ConnectionDialog::update_byte_count(int bytes) {
    m_bytes_received += bytes;
}
