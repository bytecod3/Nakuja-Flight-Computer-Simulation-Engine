#include "mainwindow.h"
#include "window_defs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // set version number
    QString version = "1.0";

    w.setMaximumSize(1400, 900);
    w.setWindowTitle("Nakuja Flight Computer Simulation Engine. v"+version);

    w.show();
    return a.exec();
}
