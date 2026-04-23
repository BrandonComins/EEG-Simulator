#include <QApplication>
#include "plotwidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Plot::PlotWidget test_window;

    test_window.setWindowTitle("Plot Widget Runner");
    test_window.show();

    return app.exec();
}