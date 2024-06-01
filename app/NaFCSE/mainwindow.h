#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "serialport.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void on_btnRun_clicked();

private:
    Ui::MainWindow *ui;
    uint8_t numBaudRates;
    uint64_t baudRates[9];
    void loadPorts();
    SerialPort port;

};
#endif // MAINWINDOW_H
