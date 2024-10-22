#include "mainwindow.h"
#include "window_defs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // set version number
    QString version = "1.0";

    // set window icon
    // w.setWindowIcon();

    w.setFixedSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    w.setWindowTitle("Nakuja Flight Computer Simulation and Testing Engine. v"+version);

    w.show();
    return a.exec();
}
