#include "connectiondialog.h"
#include "ui_connectiondialog.h"

#include <QSettings>

constexpr int default_port = 1234;

ConnectionDialog::ConnectionDialog(QWidget *parent)
    : QDialog(parent)
    , m_server_on(false)
    , m_settings(new QSettings(QStringLiteral("EEG"), QStringLiteral("Ui Connection"), this))
    , ui(new Ui::ConnectionDialog)
{
    ui->setupUi(this);

    setWindowTitle(QStringLiteral("Connection Dialog"));

    set_led_status(false);

    int saved_port = m_settings->value("last_used_port", default_port).toInt();
    ui->spinBox_port->setValue(saved_port);

    QObject::connect(ui->pushButton_connect, &QPushButton::clicked,
                     this, &ConnectionDialog::toggle_server);
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
    if(!m_server_on) {
        m_server_on = true;
        ui->pushButton_connect->setText(QStringLiteral("Stop Server"));
        Q_EMIT start_server_requested(ui->spinBox_port->value());

        // Save the last used port
        m_settings->setValue("last_used_port", ui->spinBox_port->value());
        m_settings->sync();
    } else {
        m_server_on = false;
        set_led_status(false);
        ui->pushButton_connect->setText(QStringLiteral("Start Server"));
        Q_EMIT stop_server_requested();
    }
}

void ConnectionDialog::ConnectionDialog::set_led_status(bool connected) {
    if (connected) {
        ui->label_status_led->setStyleSheet(
            "background-color: #2ecc71; "
            "border-radius: 10px; "
            "border: 2px solid #2ecc71; "
            "min-width: 20px; "
            "min-height: 20px;"
            );
    } else {
        ui->label_status_led->setStyleSheet(
            "background-color: #c0392b; "
            "border-radius: 10px; "
            "border: 2px solid #c0392b; "
            "min-width: 20px; "
            "min-height: 20px;"
            );
    }
}
