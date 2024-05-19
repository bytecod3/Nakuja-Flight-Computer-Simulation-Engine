#include "mainwindow.h"
#include "window_defs.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.resize(1024, 900);
    w.setWindowTitle("Nakuja FLight Computer Simulation Engine");

    w.show();
    return a.exec();
}
