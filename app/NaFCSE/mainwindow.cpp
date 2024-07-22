#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCharts>
#include <QSplineSeries>
#include <QSerialPortInfo>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QFileInfo>
#include "defines.h"
#include "serialparser.h"
#include "typeinfo.h"

// variables to use during handshake
// ASCII numeric values
QString NAK = "21\n"; // NAK command sent from the receiver (flight computer)
QString SOH = "1";

/**
 * @brief MainWindow::MainWindow
 * @param parent
 *
 * Constructor
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // scan and load all the serial ports
    this->loadPorts();

    // update the baud Rate combo box with possible baud rates
    QString baudRates[numBaudRates] = {"4800", "9600", "19200", "38400","57600", "115200","230400", "460800", "921600"};

    for (int i =0; i < numBaudRates; i++) {
        ui->cmbBaudRates->addItem(baudRates[i]);
    }

    // set the possible timesteps in milliseconds
    QString time_steps[numTimeSteps] = {"200", "400", "600", "800", "1000", "1500", "2000", "5000"};
    for(int64_t i =0; i < numTimeSteps; i++) {
        ui->cmbTimeStep->addItem(time_steps[i]);
    }

    // connect the timer elapse signal to update Serial function
    /* create a timer to be scanning the serial ports every 5 seconds */
    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(5000);
    mSerialScanTimer->start();

    /////////////////// SIGNALS AND SLOTS ////////////////////////
    /// 1. display data received from serial
    connect(&port, &SerialPort::dataReceived, this, &MainWindow::updateSerialMonitor);

    /// 2. process data received from the serial monitor
    connect(&port, &SerialPort::dataReceived, this, &MainWindow::readData);

    /// 3. handles plug-n-play
    connect(mSerialScanTimer, &QTimer::timeout, this, &updateSerialPorts);

    ////////////////// DRAW SPLINE CHART ///////////////////////
    QSplineSeries* series = new QSplineSeries();
    series->setName("spline");

    // add spline data points
    series->append(0,6);
    series->append(2,4);
    series->append(3,8);
    series->append(7,4);
    series->append(10,5);

    *series<<QPoint(11,1)<<QPoint(13,3)<<QPoint(17,6)<<QPoint(18,3)<<QPoint(20,2);

    // create the actual chart
    QChart* chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Acceleration");
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(0, 10);

    // create qchartview
    QChartView* chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing); // make lines look nicer

    ui->accelerationChart->addWidget(chartView);

    // create dummy graph 2
    QSplineSeries* series2 = new QSplineSeries();
    series2->setName("spline");

    // add spline data points
    series2->append(0,6);
    series2->append(2,4);
    series2->append(3,8);
    series2->append(7,4);
    series2->append(10,5);

    *series2<<QPoint(11,1)<<QPoint(13,3)<<QPoint(17,6)<<QPoint(18,3)<<QPoint(20,2);

    // create the actual chart
    QChart* chart2 = new QChart();
    chart2->legend()->hide();
    chart2->addSeries(series2);
    chart2->setTitle("Acceleration");
    chart2->createDefaultAxes();
    chart2->axes(Qt::Vertical).first()->setRange(0, 10);

    // create qchartview
    QChartView* chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing); // make lines look nicer


    ui->simulatedDataChart->addWidget(chartView2);

}

/////////////////////////////////////////////////////////////////////
//////////             SERIAL PORT FUNCTIONS             ////////////
/////////////////////////////////////////////////////////////////////

/**
 * @brief MainWindow::loadPorts
 * scan for available com ports and add them to combo box
 */
void MainWindow::loadPorts() {
    foreach(auto &port , QSerialPortInfo::availablePorts()) {
        ui->cmbSerialPorts->addItem(port.portName());
    }
}

/**
 * @brief MainWindow::updateSerialPorts
 * rescan the serial ports and update the combo box, such that when a new device
 * is connected, it is automatically detected
 */
void MainWindow::updateSerialPorts() {

    foreach(auto &port, QSerialPortInfo::availablePorts()) {
        ui->cmbSerialPorts->addItem(port.portName());
    }
}

/**
 * @brief MainWindow::updateSerialMonitor
 * @param data
 * show received data on the serial monitor of the app
 */
void MainWindow::updateSerialMonitor(const QString data) {

    // display the data on the plain text widget
    ui->serialMonitor->insertPlainText(data);
    QScrollBar* sb = ui->serialMonitor->verticalScrollBar();
    sb->setValue(sb->maximum()); // enable auto-scrolling
}

/**
 * @brief MainWindow::readData
 * @param data
 * process the data received on serial
 * this data can be anything received from serial
 */
void MainWindow::readData(QString data) {
    // TODO: parser to set some vectors
    parser.parseAll(data);

    // get type of data
    qDebug() << data.trimmed();


    // TODO: IF WE ARE IN THE HANDSHAKE STATE, parse the ASII commands from serial
    if(current_app_state == APP_STATES::HANDSHAKE) {


    } else if(current_app_state == APP_STATES::NOMINAL) {
        // update UI
        this->updateStateUI(parser.getCurrentFlightState());
    }

}

/////////////////////////////////////////////////////////////////////
//////////             BUTTON PRESS HANDLERS             ////////////
/////////////////////////////////////////////////////////////////////

/**
 * @brief MainWindow::on_btnRun_clicked
 * Run the application
 */
void MainWindow::on_btnRun_clicked(QString data)
{
    // remove the trailing newline
    data = data.trimmed();

    // check the current app state
     if(current_app_state == APP_STATES::HANDSHAKE) {
        qDebug() << "HANDSHAKE STATE";

         if(data == NAK) {
            // if NAK received, we respond with SOH to signify start of header to the MCU
            // we send this -> "21\n"
            QByteArray SOH_BYTE(QString(SOH).toUtf8());
            SOH_BYTE.append('\n');
            port.writeToSerial(SOH_BYTE);

            // at this point we should start sending test data to the MCU

            // send EOT

            // done sending data, change state to NOMINAL

        }

    } else if(current_app_state == APP_STATES::NOMINAL) {
        qDebug() << "NOMINAL STATE";
    }

    // check if the simulation data select box is empty

}

/**
 * @brief MainWindow::on_toolButton_clicked
 * Open file dialog chooser to choose trajectory file
 */
void MainWindow::on_btnChooseFile_clicked()
{
    QString trajectory_file =  QFileDialog::getOpenFileName(this, tr("Choose file"), "/", tr("CSV Files (*.csv)"));
    QFileInfo t_file(trajectory_file);
    QString file_ext = t_file.completeSuffix();
    qDebug() << file_ext;

    if (trajectory_file == "") {
        QMessageBox::critical(this, "File error", "Please select a file");
    } else if (file_ext != "csv") {
        QMessageBox::critical(this, "File error", "Please select a csv file");
        return;
    } else {
        qDebug() <<trajectory_file;
    }

    // set the filename in the filename field
    ui->lnFilename->setText(trajectory_file);

}


/**
 * @brief MainWindow::on_connectSerial_clicked
 * connect to serial port
 *
 */
void MainWindow::on_connectSerial_clicked()
{
    // connect to serial port
    QString portName = ui->cmbSerialPorts->currentText();
    QString baudRate = ui->cmbBaudRates->currentText();
    isConnected = port.connectToSerial(portName, baudRate);

    if(!isConnected) {
        QMessageBox::critical(this, "Port error", "Could not connect to port");
        // update the status bar

    } else {
        // update the status bar
        ui->statusbar->showMessage("Connected to " + portName);
        isConnected = true; // redundant
    }

}

/**
 * @brief MainWindow::on_writeSerial_clicked
 * Test writing data to serial
 */
void MainWindow::on_writeSerialButton_clicked() {
    QString serial_command = ui->serialWriteTextEntry->text();
    QByteArray serial_comm(QString(serial_command).toUtf8());
    serial_comm.append('\n');
    qDebug() << serial_comm;

    // QByteArray QB_SOH;
    // QB_SOH.setNum(SOH);
    port.writeToSerial(serial_comm);
}

/**
 * @brief MainWindow::on_closeSerial_clicked
 * Close the serial connection
 */
void MainWindow::on_closeSerialButton_clicked()
{
    // if(isConnected) {
    //     port.closeSerial();
    //     ui->statusbar->showMessage("Serial port disconnected");

    // } else {
    //     ui->statusbar->showMessage("Serial not available");
    // }

    // isConnected = false;

    this->updateSystemDiagnosticsUI();
}

/////////////////////////////////////////////////////////////////////
//////////            UI UPDATE HANDLERS                 ////////////
/////////////////////////////////////////////////////////////////////
/**
 *
 * @brief SerialParser::updateStateUI
 * @param s
 *
 * updates the state labels on screen to show change of flight states
 */
void MainWindow::updateStateUI(quint8 s) {

    // to hold labels that represent the state
    // TODO: move 10 to  #DEFINE constant
    QLabel* stateLabels[10] = {ui->preflightLabel,
                              ui->poweredflightLabel,
                              ui->coastingLabel,
                              ui->apogeeLabel,
                              ui->ballisticdescentLabel,
                              ui->droguechuteejectLabel,
                              ui->droguechutedescentLabel,
                              ui->mainchuteejectLabel,
                              ui->mainchutedescentLabel,
                              ui->postflightLabel};

    // set defaults state for the labels
    for(uint8_t i = 0; i < 10; i++) {
        stateLabels[i]->setAutoFillBackground(true);
        stateLabels[i]->setStyleSheet(" QLabel { border: 1px solid gray; border-radius: 4px; color: black; } ");
    }

    switch (s) {
    case 0:
        // qDebug() << "PREFLIGHT";
        stateLabels[0]->setStyleSheet(this->activeStateStyle);
        break;

    case 1:
        // qDebug() << "POWEREDFLIGHT";
        stateLabels[1]->setStyleSheet(this->activeStateStyle);
        break;

    case 2:
        // qDebug() << "COASTING";
        stateLabels[2]->setStyleSheet(this->activeStateStyle);
        break;

    case 3:
        // qDebug() << "APOGEE";
        stateLabels[3]->setStyleSheet(this->activeStateStyle);
        break;

    case 4:
        // qDebug() << "BALLISTIC DESCENT";
        stateLabels[4]->setStyleSheet(this->activeStateStyle);
        break;

    case 5:
        // qDebug() << "DROGUE EJECT";
        stateLabels[5]->setStyleSheet(this->activeStateStyle);
        break;

    case 6:
        // qDebug() << "DROGUE DESCENT";
        stateLabels[6]->setStyleSheet(this->activeStateStyle);
        break;

    case 7:
        // qDebug() << "MAIN CHUTE EJECT";
        stateLabels[7]->setStyleSheet(this->activeStateStyle);
        break;

    case 8:
        // qDebug() << "MAIN CHUTE DESCENT";
        stateLabels[8]->setStyleSheet(this->activeStateStyle);
        break;

    case 9:
        // qDebug() << "POST FLIGHT";
        stateLabels[9]->setStyleSheet(this->activeStateStyle);
        break;
    }


}

/**
 * @brief MainWindow::updateSystemDiagnosticsUI
 *
 * Display the status of the subsystems on the UI
 */
void MainWindow::updateSystemDiagnosticsUI() {
    // get the susbsytems labels
    QLabel* sub_system_labels[this->num_sub_systems] = {
        ui->IMU_subsys_label,
        ui->ALT_subsys_label,
        ui->GPS_subsys_label,
        ui->COMMS_subsys_label,
        ui->FLASH_subsys_label,
        ui->TESTFLASH_subsys_label,
        ui->POWER_subsys_label
    };

    // loop each susbsytem label
    // check if its value is set to 1 or 0
    // if 1, set GREEN, else set RED

    for(int j = 0; j < this->num_sub_systems; j++) {
        if(sys_diag[j] == 1) {
            sub_system_labels[j]->setAutoFillBackground(true);
            sub_system_labels[j]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
        } else {
            sub_system_labels[j]->setAutoFillBackground(true);
            sub_system_labels[j]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
        }
    }

}

/**
 * @brief MainWindow::~MainWindow
 * Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}
