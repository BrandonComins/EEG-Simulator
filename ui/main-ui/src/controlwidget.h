#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ControlWidget(const std::string &channel_id, QWidget *parent = nullptr);
    ~ControlWidget();

private:
    Ui::ControlWidget *ui;
};

#endif // CONTROLWIDGET_H
