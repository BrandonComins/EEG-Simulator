#include "scaledialog.h"

#include "ui_scaledialog.h"

Plot::ScaleDialog::ScaleDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScaleDialog) {

    ui->setupUi(this);

    this->setWindowTitle(QStringLiteral("Scale The Plot"));

    QObject::connect(ui->pushButton_auto_scale, &QPushButton::clicked, this,
                     &ScaleDialog::auto_scale_clicked, Qt::UniqueConnection);

    QObject::connect(ui->pushButton_scale, &QPushButton::clicked, this,
                     &ScaleDialog::handle_scale_button, Qt::UniqueConnection);
}

Plot::ScaleDialog::~ScaleDialog() {
    delete ui;
}

void Plot::ScaleDialog::ScaleDialog::handle_scale_button() {
    double min_x = ui->doubleSpinBox_min_x->value();
    double max_x = ui->doubleSpinBox_max_x->value();
    double min_y = ui->doubleSpinBox_min_y->value();
    double max_y = ui->doubleSpinBox_max_y->value();

    if (min_x > max_x) {
        std::swap(min_x, max_x);
    }

    if (min_y > max_y) {
        std::swap(min_y, max_y);
    }

    Q_EMIT scale_clicked(min_x, max_x, min_y, max_y);
}
