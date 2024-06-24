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
#include "serialparser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // scan and load all the serial ports
    this->loadPorts();

    // update the baud Rate combo box with possible baud rates
    this->numBaudRates = 9;
    QString baudRates[numBaudRates] = {"4800", "9600", "19200", "38400","57600", "115200","230400", "460800", "921600"};

    for (int i =0; i < numBaudRates; i++) {
        ui->cmbBaudRates->addItem(baudRates[i]);
    }


    // set the possible timesteps in milliseconds
    this->numTimeSteps = 8;
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

/**
 * @brief MainWindow::loadPorts
 * scan for available com ports and add them to combo box
 */
void MainWindow::loadPorts() {
    foreach(auto &port , QSerialPortInfo::availablePorts()) {
        ui->cmbSerialPorts->addItem(port.portName());
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_btnRun_clicked
 * Run the application
 */
void MainWindow::on_btnRun_clicked()
{
    qDebug() << "Running";

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
    // qDebug() << data;

    // display the data on the plain text widget
    ui->serialMonitor->insertPlainText(data);
    QScrollBar* sb = ui->serialMonitor->verticalScrollBar();
    sb->setValue(sb->maximum()); // enable auto-scrolling

    // get the current state
    qint8 current_state = parser.getCurrentState();

    // update UI
    this->updateStateUI(current_state);


}

/**
 * @brief MainWindow::readData
 * @param data
 * process the data received on serial
 */
void MainWindow::readData(const QString data) {
    parser.parseAll(data);
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
    auto isConnected = port.connectToSerial(portName, baudRate);

    if(!isConnected) {
        QMessageBox::critical(this, "Port error", "Could not connect to port");
        // update the status bar

    } else {
        // update the status bar
        ui->statusbar->showMessage("Connected to " + portName);

    }

}

/**
 *
 * @brief SerialParser::updateStateUI
 * @param s
 *
 * updates the state labels on screen to show change of flight states
 */
void MainWindow::updateStateUI(qint8 s) {

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
        qDebug() << "PREFLIGHT";
        ui->preflightLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 1:
        qDebug() << "POWEREDFLIGHT";
        ui->poweredflightLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 2:
        qDebug() << "COASTING";
        ui->coastingLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 3:
        qDebug() << "APOGEE";
        ui->apogeeLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 4:
        qDebug() << "BALLISTIC DESCENT";
        ui->ballisticdescentLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 5:
        qDebug() << "DROGUE EJECT";
        ui->droguechuteejectLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 6:
        qDebug() << "DROGUE DESCENT";
        ui->droguechutedescentLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 7:
        qDebug() << "MAIN CHUTE EJECT";
        ui->mainchuteejectLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 8:
        qDebug() << "MAIN CHUTE DESCENT";
        ui->mainchutedescentLabel->setStyleSheet(this->activeStateStyle);
        break;

    case 9:
        qDebug() << "POST FLIGHT";
        ui->postflightLabel->setStyleSheet(this->activeStateStyle);
        break;

    default:
        break;
    }


}
