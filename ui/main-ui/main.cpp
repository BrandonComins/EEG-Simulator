#include "mainwindow.h"

#include <QApplication>
#include <QTcpServer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle(QStringLiteral("EEG Simulator"));
    window.show();

    return app.exec();
}
