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
#include <fstream>
#include <bitset>
#include "defines.h"
#include "serialparser.h"
#include "typeinfo.h"
#include "csv-parser.h"

// variables to use during handshake
// ASCII numeric values
QString NAK = "21\n"; // NAK command sent from the receiver (flight computer) - device under test
QString SOH = "1\n";

QString system_state = "";

/**
 * @brief MainWindow::MainWindow
 * @param parent
 *
 * Constructor
 *
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set up button icons
    QPixmap pixmap("./icons/play.png");
    QIcon ButtonIcon(pixmap);
    ui->btnLink->setIcon(ButtonIcon);

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

    // create a timer to be updating the system init mask every 2 seconds
    updateSubSystemsTimer = new QTimer(this);
    updateSubSystemsTimer->setInterval(1000);
    updateSubSystemsTimer->start();

    /////////////////// SIGNALS AND SLOTS ////////////////////////
    /// display data received from serial
    connect(&port, &SerialPort::dataReceived, this, &MainWindow::updateSerialMonitor);

    /// process data received from the serial monitor
    connect(&port, &SerialPort::dataReceived, this, &MainWindow::readData);

    /// handles plug-n-play for serial port
    connect(mSerialScanTimer, &QTimer::timeout, this, &updateSerialPorts);

    // update the susbsystems system periodically
    connect(updateSubSystemsTimer, &QTimer::timeout, this, &updateSubSystemsPeriodic);

    // on data transmission end
    connect(this, &MainWindow::endOfTransmissionSignal, this, &MainWindow::handleEndOfTransmission);

    ////////////////// INIT PLOT AREA ///////////////////////
    // ui->plotWidget->resize(300, 200);
    // this->initPlotArea();

    // check our current app state to display on status bar
    if(current_app_state == APP_STATES::HANDSHAKE) {
        ui->statusbar->showMessage("STATE: HANDSHAKE");
    } else if (current_app_state == APP_STATES::HANDSHAKE){
        ui->statusbar->showMessage("STATE: NOMINAL");
    } else if (current_app_state == APP_STATES::RUNNING) {
        ui->statusbar->showMessage("STATE: RUNNING");
    } else {
        ui->statusbar->showMessage("Waiting");
    }


    // SET STATIC UI
    this->setStaticUI();

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
    // parser.parseAll(data);

    // check system
    if(current_app_state == APP_STATES::SYSTEM_CHECK) {
        QString state = data.trimmed();
        system_state = state;
        //systemsCheck(system_state);

        // reset the state
        current_app_state = APP_STATES::NOMINAL;
    }

    if(data.trimmed() == "EOT") {
        emit endOfTransmissionSignal();
    }

    // check for states
    QString stte = "";
    if(data.trimmed() == "PREFLIGHT") {
        stte = "PREFLIGHT";
    } else if(data.trimmed() == "POWERED") {
        stte = "POWERED";
    } else if(data.trimmed() == "COASTING") {
        stte = "COASTING";
    } else if(data.trimmed() == "APOGEE") {
        stte = "APOGEE";
    } else if(data.trimmed() == "BALLISTIC") {
        stte = "BALLISTIC";
    } else if(data.trimmed() == "DROGUE") {
        stte = "DROGUE";
    } else if(data.trimmed() == "DROGUE_DESC") {
        stte = "DROGUE_DESC";
    } else if(data.trimmed() == "MAIN") {
        stte = "MAIN";
    } else if(data.trimmed() == "MAIN_DESC") {
        stte = "MAIN_DESC";
    } else if(data.trimmed() == "POST_FLIGHT") {
        stte = "POST_FLIGHT";
    } else if(data.trimmed() == "END") {
        stte = "END";
    }

    updateFlightStateUI(stte);

    // char* data_char =data.toStdString().c_str();
    // qDebug() << data;
    // if(data == "EOT\n") {
    //     // emit endOfTransmissionSignal();
    //     qDebug() << "End of transmission received from DUT";
    // }

    // convert back to QString
    //QString data_str = QString::fromUtf16((ushort*)(data_char));

    // TODO: IF WE ARE IN THE HANDSHAKE STATE, parse the ASCII commands from serial
    // if(current_app_state == APP_STATES::HANDSHAKE) {
    //     qDebug() << current_app_state;
    // } else if(current_app_state == APP_STATES::NOMINAL) {
    //     //this->updateStateUI(parser.getCurrentFlightState());
    //     qDebug() << current_app_state;
    // } else if(current_app_state == APP_STATES::SYSTEM_CHECK) {
    //     // check the sub-systems init mask
    //     QString state = data.trimmed();
    //     system_state = state;
    // }
}

void MainWindow::systemsCheck(QString s) {
    bool ok;
    int bit_length = 8;
    int string_length = 0;
    int s_dec = s.toInt(&ok, 10);

    // convert to binary
    std::bitset<8> s_bin = s_dec;
    QString st = QString::number(s_dec, 2);

    string_length = st.length();

    const QChar p[1] = {'0'};

    // improve this code to make it handle bit length of any size
    if(string_length == 6) {
        st.insert(0, QString("0"));
        st.insert(1, QString("0"));
    } else {
        st.insert(0, p, (bit_length-string_length)); // overloaded
    }

    this->updateSystemDiagnosticsUI(st);

}

/////////////////////////////////////////////////////////////////////
//////////             BUTTON PRESS HANDLERS             ////////////
/////////////////////////////////////////////////////////////////////

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
        // qDebug() <<trajectory_file;
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
    QString portName = ui->cmbSerialPorts->currentText(); // get the selected COM port
    QString baudRate = ui->cmbBaudRates->currentText(); // get the selected BAUD rate

    if(!isConnected) {
        isConnected = port.connectToSerial(portName, baudRate); // try serial connection
        ui->statusbar->showMessage("Connected to " + portName);
        isConnected = true;
        // QMessageBox::critical(this, "Port error", "Could not connect to port");

    } else {
        // update the status bar
        // ui->statusbar->showMessage("Connected to " + portName);

        // disable the serial connect button
        // ui->connectSerial->setEnabled(false);
        // isConnected = true; // redundant
        QMessageBox::critical(this, "Port error", "Could not connect to port");
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
    // qDebug() << serial_comm;

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
    if(isConnected) {

        port.closeSerial();
        isConnected = false;
        ui->statusbar->showMessage("Serial port disconnected");

    } else {
        ui->statusbar->showMessage("Serial not available");
    }
}

void MainWindow::setStaticUI() {

    // overall app background
    ui->centralwidget->setAutoFillBackground(true);
    ui->centralwidget->setStyleSheet("background-color: #2f2f2f; ");

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
        stateLabels[i]->setStyleSheet("QLabel { padding: 2px; background-color : #1a2e49; color : #ffffff; border-radius: 2px; }");
    }

    // Flight states group box styles
    ui->flightStatesGroupBox->setAutoFillBackground(true);
    ui->flightStatesGroupBox->setStyleSheet("QGroupBox { background-color: #111111; color: #ffffff; border: 2px solid #111111; padding: 5px; border-radius: 2px; } ");

    // Simulation parameters group box colors
    ui->simulationParamsGroupBox->setAutoFillBackground(true);
    ui->simulationParamsGroupBox->setStyleSheet("QGroupBox { background-color: #111111; color: #ffffff; border: 2px solid #111111; padding: 5px; border-radius: 2px; } ");

    // Simulation parameters labels and select boxes
    ui->lblPort->setAutoFillBackground(true);
    ui->lblPort->setStyleSheet("QLabel { background-color: #111111; color: #ffffff; }");
    ui->lblBaudRate->setAutoFillBackground(true);
    ui->lblBaudRate->setStyleSheet(" QLabel {background-color: #111111; color: #ffffff; }");
    ui->lblSimulationFile->setAutoFillBackground(true);
    ui->lblSimulationFile->setStyleSheet(" QLabel {background-color: #111111;  color: #ffffff; }");
    ui->lblTimeStep->setAutoFillBackground(true);
    ui->lblTimeStep->setStyleSheet(" QLabel {background-color: #111111;  color: #ffffff; }");
    ui->cmbSerialPorts->setAutoFillBackground(true);
    ui->cmbSerialPorts->setStyleSheet("QComboBox { background-color: #244976; color: #ffffff; } ");
    ui->cmbBaudRates->setAutoFillBackground(true);
    ui->cmbBaudRates->setStyleSheet("QComboBox { background-color: #244976; color: #ffffff; } ");
    ui->lnFilename->setAutoFillBackground(true);
    ui->lnFilename->setStyleSheet("QLineEdit { background-color: #ffffff; color: #1a2e49; } ");
    ui->cmbTimeStep->setAutoFillBackground(true);
    ui->cmbTimeStep->setStyleSheet("QComboBox { background-color: #244976; color: #ffffff; } ");
    ui->btnChooseFile->setAutoFillBackground(true);
    ui->btnChooseFile->setStyleSheet("QToolButton { background-color: #244976; color: #ffffff; } ");

    // serial connect buttons
    ui->connectSerial->setAutoFillBackground(true);
    ui->connectSerial->setStyleSheet(" QPushButton { background-color: #000017; color: #ffffff; } ");
    ui->closeSerialButton->setAutoFillBackground(true);
    ui->closeSerialButton->setStyleSheet(" QPushButton { background-color: #dc1300; color: #ffffff; } " );

    // sub-systems check UI
    // initially the sub-systems label is greyed out
    // if a system is found faulty , the bg color changes to red
    // othersiw if it is okay, the bg color changes to green
    ui->IMU_subsys_label->setAutoFillBackground(true);
    ui->IMU_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->ALT_subsys_label->setAutoFillBackground(true);
    ui->ALT_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->GPS_subsys_label->setAutoFillBackground(true);
    ui->GPS_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->COMMS_subsys_label->setAutoFillBackground(true);
    ui->COMMS_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->FLASH_subsys_label->setAutoFillBackground(true);
    ui->FLASH_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->SD_subsys_label->setAutoFillBackground(true);
    ui->SD_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    ui->SPIFFS_subsys_label->setAutoFillBackground(true);
    ui->SPIFFS_subsys_label->setStyleSheet( "QLabel { padding: 2px; background-color : #958d93; color : #ffffff; border-radius: 2px; }" );

    // flight systems groupbox styles
    ui->flightSystemsGroupBox->setAutoFillBackground(true);
    ui->flightSystemsGroupBox->setStyleSheet( "QGroupBox { background-color: #111111; color: #ffffff; border: 2px solid #111111; padding: 5px; border-radius: 2px; } ");

    // Software mode group
    ui->softwareModeGroupBox->setAutoFillBackground(true);
    ui->softwareModeGroupBox->setStyleSheet("QGroupBox { background-color: #111111; color: #ffffff; border: 2px solid #111111; padding: 5px; border-radius: 2px; } ");

    // ejection stages groupbox
    ui->EjectionStagesGroupBox->setAutoFillBackground(true);
    ui->EjectionStagesGroupBox->setStyleSheet("QGroupBox { background-color: #111111; color: #ffffff; border: 2px solid #111111; padding: 5px; border-radius: 2px; } ");

    // software mode labels
    ui->lblFlightMode->setAutoFillBackground(true);
    ui->lblFlightMode->setStyleSheet(" QLabel { padding: 2px; background-color : #1a2e49; color : #ffffff; border-radius: 2px; } ");
    ui->lblSafeMode->setAutoFillBackground(true);
    ui->lblSafeMode->setStyleSheet( " QLabel { padding: 2px; background-color : #1a2e49; color : #ffffff; border-radius: 2px; } ");

    // ejection stages radio buttons
    ui->radioSingleEjection->setAutoFillBackground(true);
    ui->radioSingleEjection->setStyleSheet(" QRadioButton { padding: 2px; background-color : #111111; color : #ffffff; border-radius: 2px; } ");
    ui->radioDoubleEjection->setAutoFillBackground(true);
    ui->radioDoubleEjection->setStyleSheet(" QRadioButton { padding: 2px; background-color : #111111; color : #ffffff; border-radius: 2px; } ");

    // data link button
    ui->btnLink->setAutoFillBackground(true);
    ui->btnLink->setStyleSheet(" QPushButton { background-color: #000017; color: #ffffff;padding:10px; border: 1px solid gray; border-radius: 2px;} ");

    // simulation button test
    ui->btnMainRun->setAutoFillBackground(true);
    ui->btnMainRun->setStyleSheet(" QPushButton { background-color: #000017; color: #ffffff; padding:10px; border: 1px solid green; border-radius: 2px;} ");

    ui->btnCheckSystems->setAutoFillBackground(true);
    ui->btnCheckSystems->setStyleSheet(" QPushButton { background-color: #000017; color: #ffffff;padding:10px; border: 1px solid gray; border-radius: 2px;} ");

    // simulated altitude data plot
    ui->lblSimulatedAlt->setAutoFillBackground(true);
    ui->lblSimulatedAlt->setStyleSheet(" QLabel { background-color: #111111; color: #ffffff; padding: 2px; }  " );

    // simulated pressure data plot
    ui->lblSimulatedPressure->setAutoFillBackground(true);
    ui->lblSimulatedPressure->setStyleSheet(" QLabel { background-color: #111111; color: #ffffff; padding: 2px;}  " );

    // serial monitor
    ui->serialWriteTextEntry->setAutoFillBackground(true);
    ui->serialWriteTextEntry->setStyleSheet(" QLineEdit { background-color: #ffffff; color: #1a2e49; padding: 2px; }  ");
    ui->writeSerialButton->setAutoFillBackground(true);
    ui->writeSerialButton->setStyleSheet(" QPushButton { background-color: #1c2e50; color: #ffffff; padding; 4px; } ");
    ui->lblSerialMonitor->setAutoFillBackground(true);
    ui->lblSerialMonitor->setStyleSheet("QLabel { color: #ffffff; } ");

    // progress bar
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0, 2000);
    ui->progressBar->setMaximum(10000);

    ui->serialMonitor->setAutoFillBackground(true);
    ui->serialMonitor->setStyleSheet(" QTextBrowser { background-color: #050505; color: #1fa61c; border: 1px solid gray; } ");

    // status bar
    ui->statusbar->setAutoFillBackground(true);
    ui->statusbar->setStyleSheet(" QStatusBar {  background-color: #2f2f2f; color: #ffffff; } ");

    // menubar
    ui->menubar->setAutoFillBackground(true);
    ui->menubar->setStyleSheet( " QMenuBar {  background-color: #2f2f2f; color: #ffffff; }  " );


}

/////////////////////////////////////////////////////////////////////
//////////            UI UPDATE HANDLERS                 ////////////
/////////////////////////////////////////////////////////////////////

/**
 * @brief MainWindow::updateSystemDiagnosticsUI
 *
 * Display the status of the subsystems on the UI
 */
void MainWindow::updateSystemDiagnosticsUI(QString st) {
    // get the subsystems labels
    QLabel* sub_system_labels[this->num_sub_systems] = {
        ui->ALT_subsys_label,
        ui->IMU_subsys_label,
        ui->FLASH_subsys_label,
        ui->GPS_subsys_label,
        ui->SD_subsys_label, // sd card
        ui->SPIFFS_subsys_label, // SPIFFS
        ui->TESTGPIO_subsys_label, // TEST GPIO
        ui->COMMS_subsys_label, // No way of testing comms at the moment

    };

    // loop each susbsytem label
    // check if its value is set to 1 or 0
    // if 1, set GREEN, else set RED

    // set the subsytems init UI
    /** bit assignment
     * bit0 -> ALTIMETER          (LSB)
     * bit1 -> IMU
     * bit2 -> FLASH
     * bit3 -> GPS
     * bit4 -> SD CARD
     * bit5 -> SPIFFS
     * bit6 -> TESTGPIO
     * bit7 -> X (unused)   (MSB) // reserved for COMMS (xbee)
     *
    */

    // check ALTIMETER
    if(QString::compare("1", st[7]) == 0) {
        sub_system_labels[0]->setAutoFillBackground(true);
        sub_system_labels[0]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[0]->setAutoFillBackground(true);
        sub_system_labels[0]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check IMU
    if(QString::compare("1", st[6]) == 0) {
        sub_system_labels[1]->setAutoFillBackground(true);
        sub_system_labels[1]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[1]->setAutoFillBackground(true);
        sub_system_labels[1]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check FLASH
    if(QString::compare("1", st[5]) == 0) {
        sub_system_labels[2]->setAutoFillBackground(true);
        sub_system_labels[2]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[2]->setAutoFillBackground(true);
        sub_system_labels[2]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check GPS
    if(QString::compare("1", st[4]) == 0) {
        sub_system_labels[3]->setAutoFillBackground(true);
        sub_system_labels[3]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[3]->setAutoFillBackground(true);
        sub_system_labels[3]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check SD CARD
    if(QString::compare("1", st[3]) == 0) {
        sub_system_labels[4]->setAutoFillBackground(true);
        sub_system_labels[4]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[4]->setAutoFillBackground(true);
        sub_system_labels[4]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check SPIFFS
    if(QString::compare("1", st[2]) == 0) {
        sub_system_labels[5]->setAutoFillBackground(true);
        sub_system_labels[5]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[5]->setAutoFillBackground(true);
        sub_system_labels[5]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

    // check TEST-GPIO
    if(QString::compare("1", st[1]) == 0) {
        sub_system_labels[6]->setAutoFillBackground(true);
        sub_system_labels[6]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: green } ");
    } else {
        sub_system_labels[6]->setAutoFillBackground(true);
        sub_system_labels[6]->setStyleSheet("  QLabel { border: 1px solid gray; border-radius: 4px; color: black; background: red } ");
    }

}

void MainWindow::updateFlightStateUI(QString state) {
    // TODO: move 10 to  #DEFINE constant
    QLabel* stateLabels[10] = {
       ui->preflightLabel,
       ui->poweredflightLabel,
       ui->coastingLabel,
       ui->apogeeLabel,
       ui->ballisticdescentLabel,
       ui->droguechuteejectLabel,
       ui->droguechutedescentLabel,
       ui->mainchuteejectLabel,
       ui->mainchutedescentLabel,
       ui->postflightLabel
    };

    if(state == "PREFLIGHT") {
        stateLabels[0]->setAutoFillBackground(true);
        stateLabels[0]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "POWERED" ) {
        stateLabels[1]->setAutoFillBackground(true);
        stateLabels[1]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "COASTING" ) {
        stateLabels[2]->setAutoFillBackground(true);
        stateLabels[2]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    }else if( state == "APOGEE" ) {
        stateLabels[3]->setAutoFillBackground(true);
        stateLabels[3]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "BALLISTIC" ) {
        stateLabels[4]->setAutoFillBackground(true);
        stateLabels[4]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "DROGUE" ) {
        stateLabels[5]->setAutoFillBackground(true);
        stateLabels[5]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "DROGUE_DESC" ) {
        stateLabels[6]->setAutoFillBackground(true);
        stateLabels[6]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    }else if( state == "MAIN" ) {
        stateLabels[7]->setAutoFillBackground(true);
        stateLabels[7]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "MAIN_DESC" ) {
        stateLabels[8]->setAutoFillBackground(true);
        stateLabels[8]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if( state == "POST_FLIGHT" ) {
        stateLabels[9]->setAutoFillBackground(true);
        stateLabels[9]->setStyleSheet("QLabel { padding: 2px; background-color : #F98E0E; color : #ffffff; border-radius: 2px; }");
    } else if(state == "END") {
        // set defaults state for the labels
        for(uint8_t i = 0; i < 10; i++) {
            stateLabels[i]->setAutoFillBackground(true);
            stateLabels[i]->setStyleSheet("QLabel { padding: 2px; background-color : #1a2e49; color : #ffffff; border-radius: 2px; }");
        }
    }

}

/**
 * @ brief Setup graphing widget on startup
 */
// void MainWindow::initPlotArea() {
//     // plot a simple quadratic graph
//     QVector<double> x(101), y(101);
//     for (int i=0; i< 101; ++i) {
//         x[i] = i/50.0 - 1;
//         y[i] = x[i]*x[i]; // quadratic function
//     }

//     // create a graph and assign data to it
//     ui->plotWidget->addGraph();
//     ui->plotWidget->graph(0)->setData(x, y);

//     // label the axes
//     ui->plotWidget->xAxis->setLabel("X value");
//     ui->plotWidget->yAxis->setLabel("Y label");

//     // set the axes range, so we see all data
//     ui->plotWidget->xAxis->setRange(-1, 1);
//     ui->plotWidget->yAxis->setRange(0, 1);

//     ui->plotWidget->replot();

// }

/**
 * @brief MainWindow::~MainWindow
 * Destructor
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief MainWindow::on_btnLink_clicked
 * Establish handshake with the flight computer hardware via XMODEM
 * @param data command received from serial port from the device under test
 */
void MainWindow::on_btnLink_clicked()
{

    if(current_app_state == APP_STATES::HANDSHAKE) {
        // send SOH signal to the device under test Start of Header
        QByteArray soh_byte = SOH.toUtf8();
        this->port.writeToSerial(soh_byte);
        qDebug() << soh_byte;
    }

    // get the response command from serial
    // here, the state has changed to
    // invoke the csv-parser

    // get the simulation data file   
    QString data_file = ui->lnFilename->text();
    QFileInfo t_file(data_file);
    QString file_ext = t_file.completeSuffix();

    // check if a simulation file has been selected
    if(data_file == "") {
        QMessageBox::critical(this, "File error", "Please select a file");
    } else if(file_ext != "csv") {
        QMessageBox::critical(this, "File error", "Please select a csv file");
    } else {

        // right file selected
        QByteArray filename = data_file.toLocal8Bit();
        const char* file_str = filename.data();

        // vectors to hold the simulation data values
        QVector<double> altitude;       // hold the altitude values
        QVector<double> ax;             // x accelerationl
        QVector<double> ay;             // y acceleration
        QVector<double> az;             // z acceleration
        QVector<double> lat;            // gps latitude
        QVector<double> longt;          // gps longitude
        QVector<double> atm_pressure;   // atmospheric pressure

        // open simulation data file
        std::ifstream f(file_str);

        ////////////////////////////////////////////////////////////////////
        /// // process x acceleration data
        // row represents one row of data
        // CSVRow row;

        // // from the order of the csv file, first column == x acceleration
        // // second column is the altitude
        // std::vector<std::string> x_accel;
        // while(f >> row) {
        //     x_accel.push_back(row[0]);
        // }

        // // feed this into the QVector to prepare for transmission
        // for(const auto& element: x_accel) {
        //     QString element_qs = QString::fromLocal8Bit(element.c_str());
        //     ax.push_back(element_qs.toDouble());
        // }

        // // send the data to device under test - TODO: remove this line here
        // // get length of the data points
        // int vec_length = ax.size();
        // for(int i = 0; i < vec_length; i++) {
        //     qDebug() << ax[i];
        // }

        // Process altitude data
        CSVRow row;
        std::vector<std::string> altitude_string;
        while(f >> row) {
            altitude_string.push_back(row[1]);
        }

        // feed the altitude data into the QVector
        for(const auto& element: altitude_string) {
            QString element_qs = QString::fromLocal8Bit(element.c_str());
            altitude.push_back(element_qs.toDouble());
        }

        int alt_vec_length = altitude.size();

        // for(int i=0; i < alt_vec_length; i++) {
        //     // qDebug() << altitude[i];
        //     // write the altitude to the serial port
        //     QString alt_str = QString::number(altitude[i]);
        //     QByteArray altitude_to_serial_int(QString(alt_str).toUtf8());
        //     altitude_to_serial_int.append('\n');
        //     port.writeToSerial(altitude_to_serial_int); // Send data to serial

        //     // update the progress bar
        //     ui->progressBar->setValue(i);
        // }

        int vl = 0;
        while (vl != alt_vec_length) {
            // write the altitude to the serial port
            QString alt_str = QString::number(altitude[vl]);
            QByteArray altitude_to_serial_int(QString(alt_str).toUtf8());
            altitude_to_serial_int.append('\n');

            // qDebug() << "SENDINTO SERIAL";
            port.writeToSerial(altitude_to_serial_int); // Send data to serial

            vl++;
            // qDebug() << vl;

        }


        ////////////////////////////////////////////////////////////////////

        // plot the data on the app

        // close the file

    }

}

/**
 * @brief MainWindow::handleEndOfTransmission
 * Alert on End of Transmission
 */
void MainWindow::handleEndOfTransmission()
{
    qDebug() << "EOT signal received from Device under test";

    // pop-up message
    QMessageBox::information(this,  "Data link", "End of Transmission to device under test.");
}

/**
 * @brief MainWindow::handleSubsystemsCheck
 * Receives the string containing the init status of the flight computer subsystems
 * and updates the UI
 */
void MainWindow::handleSubsystemsCheck() {

    qDebug() << "Subsystems check";
}

void MainWindow::handleStateReceive() {
    qDebug() << "Change of state";
}

void MainWindow::on_btnCheckSystems_clicked()
{
    current_app_state = APP_STATES::SYSTEM_CHECK;
    QString reset_command = "1"; // sends command 1 to the flight computer to check for subsystems
    QByteArray reset_cmd(reset_command.toUtf8());
    reset_cmd.append('\n');
    port.writeToSerial(reset_cmd);

}


void MainWindow::updateSubSystemsPeriodic() {
    bool ok;
    int bit_length = 8;
    int string_length = 0;
    int s_dec = system_state.toInt(&ok, 10);

    // convert to binary
    std::bitset<8> s_bin = s_dec;
    QString st = QString::number(s_dec, 2);

    string_length = st.length();

    const QChar p[1] = {'0'};

    // improve this code to make it handle bit length of any size
    if(string_length == 6) {
        st.insert(0, QString("0"));
        st.insert(1, QString("0"));
    } else {
        st.insert(0, p, (bit_length-string_length)); // overloaded
    }

    //qDebug() << st;
    this->updateSystemDiagnosticsUI(st);

}
