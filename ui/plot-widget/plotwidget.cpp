#include "plotwidget.h"
#include "ui_plotwidget.h"

#include "plothelper.h"

#include <qwt_plot.h>
#include<QFile>

Plot::PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
    , m_plot(nullptr)
    , ui(new Ui::PlotWidget) {

    ui->setupUi(this);

    m_plot = new PlotHelper(ui->frame, this);

    if (ui->frame->layout() == nullptr) {
        QVBoxLayout* layout = new QVBoxLayout(ui->frame);
        layout->setContentsMargins(0, 0, 0, 0);
        // layout->addWidget(m_plot->plot());
    }

    m_plot->add_series("Channel 0");
    m_plot->auto_scale();
}

Plot::PlotWidget::~PlotWidget() {
    delete ui;
}
