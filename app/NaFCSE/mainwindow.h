#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "serialport.h"
#include <QTimer> // to scan the ports periodically

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
    void on_btnChooseFile_clicked();
    void readData(QByteArray data);

private:
    Ui::MainWindow *ui;
    uint8_t numBaudRates;
    uint64_t baudRates[9];
    uint64_t numTimeSteps;
    SerialPort port;
    QTimer* mSerialScanTimer; // to scan the serial ports periodically
    void loadPorts();
    void updateSerialPorts();
};
#endif // MAINWINDOW_H
