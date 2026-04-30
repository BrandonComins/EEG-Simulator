#include "stylecurvediag.h"
#include "ui_stylecurvediag.h"

#include <QColorDialog>
#include <QSignalBlocker>

Plot::StyleCurveDiag::StyleCurveDiag(const std::string &id,
                                     Plot::CurveConfig &live_config,
                                     QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::StyleCurveDiag)
    , m_id(id)
    , m_live_config(live_config)
    , m_backup_config(live_config)
{
    ui->setupUi(this);
    this->setWindowTitle(QString::fromStdString(id) + " Style Settings");

    sync_ui_to_config();

    QObject::connect(ui->doubleSpinBox_line_width, qOverload<double>(&QDoubleSpinBox::valueChanged),
            this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->checkBox_antialiasing, &QCheckBox::toggled,
            this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->spinBox_z_order, &QSpinBox::valueChanged,
                     this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->pushButton_save, &QPushButton::clicked,
                     this, &StyleCurveDiag::on_save_clicked);

    QObject::connect(ui->pushButton_revert, &QPushButton::clicked,
                     this, &StyleCurveDiag::on_cancel_clicked);

    QObject::connect(ui->comboBox_line_style, &QComboBox::activated,
                     this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->comboBox_curve_style, &QComboBox::activated,
                     this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->comboBox_symbol, &QComboBox::activated,
                     this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->doubleSpinBox_y_offset, &QDoubleSpinBox::valueChanged,
                     this, &StyleCurveDiag::on_ui_changed);

    QObject::connect(ui->pushButton_color, &QPushButton::clicked, [&] {
        QColor color = QColorDialog::getColor(m_live_config.color, this);
        if (color.isValid()) {
            QString qss = QString("background-color: %1;").arg(color.name());
            ui->pushButton_color->setStyleSheet(qss);
            m_live_config.color = color;
            on_ui_changed();
        }
    });

    sync_ui_to_config();
}

Plot::StyleCurveDiag::~StyleCurveDiag() {
    delete ui;
}

void Plot::StyleCurveDiag::sync_ui_to_config() {
    QSignalBlocker blocker(this);

    QString qss = QString("background-color: %1;").arg(m_live_config.color.name());
    ui->pushButton_color->setStyleSheet(qss);

    ui->checkBox_antialiasing->setChecked(m_live_config.use_antialiasing);
    ui->spinBox_z_order->setValue(m_live_config.z_order);
    ui->doubleSpinBox_y_offset->setValue(m_live_config.y_offset);
    ui->doubleSpinBox_line_width->setValue(static_cast<double>(m_live_config.line_width));
    ui->comboBox_line_style->setCurrentIndex(static_cast<int>(m_live_config.line_style));
    ui->comboBox_curve_style->setCurrentIndex(static_cast<int>(m_live_config.curve_style) + 1);
    ui->comboBox_symbol->setCurrentIndex(static_cast<int>(m_live_config.symbol_type) + 1);
}

void Plot::StyleCurveDiag::on_ui_changed() {
    //Color is controlled by the color button's slot
    m_live_config.use_antialiasing = ui->checkBox_antialiasing->isChecked();
    m_live_config.z_order          = ui->spinBox_z_order->value();
    m_live_config.y_offset         = ui->doubleSpinBox_y_offset->value();
    m_live_config.line_width       = static_cast<float>(ui->doubleSpinBox_line_width->value());
    m_live_config.line_style       = static_cast<Qt::PenStyle>(ui->comboBox_line_style->currentIndex());
    m_live_config.curve_style      = static_cast<QwtPlotCurve::CurveStyle>(ui->comboBox_curve_style->currentIndex() - 1);
    m_live_config.symbol_type      = static_cast<QwtSymbol::Style>(ui->comboBox_symbol->currentIndex() - 1);

    Q_EMIT settings_changed(m_id);
}

void Plot::StyleCurveDiag::on_save_clicked() {
    m_backup_config = m_live_config;

    this->close();
}

void Plot::StyleCurveDiag::on_cancel_clicked() {
    m_live_config = m_backup_config;
    sync_ui_to_config();

    Q_EMIT settings_changed(m_id);
}