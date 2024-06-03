#include "serialport.h"
#include <QDebug>

SerialPort::SerialPort(QObject *parent)
    : QObject{parent},
    _serialPort(nullptr)
{

    /* create a timer to be scanning the serial ports every 5 seconds */
    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(5000);
    mSerialScanTimer->start();

    connect(mSerialScanTimer, &QTimer::timeout,
            this, &updateSerialPorts);
}


/**
 * @brief SerialPort::scanSerial
 * scan the available serial ports
 */
bool SerialPort::connectToSerial(QString portName, QString baudRate) {
    // check is the port is not open
    if (_serialPort!= nullptr) {
        _serialPort->close();
        delete(_serialPort);
    }

    // initialize the serial port
    _serialPort = new QSerialPort(this);
    _serialPort->setPortName(portName);
    _serialPort->setDataBits(QSerialPort::Data8);
    _serialPort->setParity(QSerialPort::NoParity);
    _serialPort->setStopBits(QSerialPort::OneStop);
    _serialPort->setBaudRate(baudRate.toInt());

    if(_serialPort->open(QIODevice::ReadWrite)) {
        // QObject::connect(_serialPort, &QSerialPort::readyRead,
        //                  this, &SerialPort::dataReady);
        qDebug() << "Connected";
    }

    return _serialPort->isOpen();

}
