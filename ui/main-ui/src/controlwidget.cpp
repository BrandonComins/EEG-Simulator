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
