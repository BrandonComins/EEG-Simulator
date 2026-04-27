#include <QApplication>
#include <QTimer>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <cmath>
#include "plothelper.h"

int timeouts = 0;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    constexpr int interval_ms = 15;
    auto *timer = new QTimer();
    timer->setInterval(interval_ms);

    QWidget runner;
    runner.setWindowTitle(QStringLiteral("Plot Runner"));
    runner.resize(800, 600);

    auto *main_layout = new QGridLayout(&runner);

    auto *plot_container = new QFrame();
    plot_container->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    main_layout->addWidget(plot_container);

    auto *plot_helper = new Plot::PlotHelper(plot_container);

    plot_helper->add_series("Sine");
    plot_helper->add_series("Cosine");
    plot_helper->set_scale_x(0, 300);
    plot_helper->set_axis_titles("Samples", "Amplitude");
    plot_helper->set_view_mode(Plot::PlotMode::ROLLING);

    QObject::connect(timer, &QTimer::timeout, [&]{
        double sine_y = std::sin(timeouts * 0.1);
        double cosine_y = std::cos(timeouts * 0.1);

        plot_helper->add_point("Sine", static_cast<double>(timeouts), sine_y);
        plot_helper->add_point("Cosine", static_cast<double>(timeouts), cosine_y);
        timeouts += 1;
    });

    runner.show();
    timer->start();

    return app.exec();
}