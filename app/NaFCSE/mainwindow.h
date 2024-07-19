#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "serialport.h"
#include "serialparser.h"
#include <QTimer>


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
    int SERIAL_BUFF_LENGTH = 256;
    QString activeStateStyle = "QLabel { background-color : black; color : #00FF00; border: 1px solid gray; border-radius: 4px; font: 700 9pt; }";
    ~MainWindow();

private slots:
    void on_btnRun_clicked(QString);
    void on_btnChooseFile_clicked();
    void updateSerialMonitor(const QString data);
    void updateStateUI(quint8 state);
    void readData(const QString data);

    void on_connectSerial_clicked();

    void on_writeSerialButton_clicked();

    void on_closeSerialButton_clicked();

private:
    Ui::MainWindow *ui;
    uint8_t numBaudRates;
    uint64_t baudRates[9];
    uint64_t numTimeSteps;
    SerialPort port;
    bool isConnected = false;
    SerialParser parser;
    QTimer* mSerialScanTimer; // to scan the serial ports periodically
    void loadPorts();
    void updateSerialPorts();
};
#endif // MAINWINDOW_H
