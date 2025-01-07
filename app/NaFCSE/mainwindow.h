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
    void setStaticUI();

private slots:
    // serial monitor functions
    void updateSerialMonitor(const QString data);
    void readData(const QString data);

    // button press handlers
    void on_btnChooseFile_clicked();
    void on_connectSerial_clicked();
    void on_writeSerialButton_clicked();
    void on_closeSerialButton_clicked();
    void on_btnLink_clicked();
    void on_btnCheckSystems_clicked();

    // update UI functions
    void updateFlightStateUI(QString);
    void updateSystemDiagnosticsUI(QString);
    void updateSubSystemsPeriodic();

    // EOT slots
    void handleEndOfTransmission();

    // subsystems check slot
    void handleSubsystemsCheck();
    void handleStateReceive();

    // graphing capabilities
    // void initPlotArea();

    void systemsCheck(QString);


signals:
    void endOfTransmissionSignal(); // to check for End of Transmission signal from DUT
    void subsystemsInitCheckSignal(); // to check for subsystems
    void preflightRCVDSignal();
    void poweredFlightRCVDSignal();
    void apogeeRCVDSignal();

private:
    Ui::MainWindow *ui;
    static constexpr uint8_t numBaudRates = 9;
    uint64_t baudRates[9];
    static constexpr uint64_t numTimeSteps = 8;
    SerialPort port;
    bool isConnected = false;
    SerialParser parser;
    QTimer* mSerialScanTimer; // to scan the serial ports periodically
    void loadPorts();
    void updateSerialPorts();

    QTimer* updateSubSystemsTimer;

    // test
    // order -> imu, altimeter, gps, comms, flash, test-flash, power
    static constexpr int num_sub_systems = 8;
    int sys_diag[num_sub_systems] = {1,1,1,1,0,1,0};

};
#endif // MAINWINDOW_H
