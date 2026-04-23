#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

namespace Ui {
class PlotWidget;
}

namespace Plot {
class PlotHelper;

class PlotWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = nullptr);
    ~PlotWidget();

private:
    PlotHelper *m_plot;
    Ui::PlotWidget *ui;
};
}; //namespace Plot

#endif // PLOTWIDGET_H
