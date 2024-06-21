#include "serialport.h"
#include <QDebug>

SerialPort::SerialPort(QObject *parent)
    : QObject{parent},
    _serialPort(nullptr)
{


}

/**
 * @brief SerialPort::~serialPort
 *
 * Destructor - close any open serial port
 */
SerialPort::~SerialPort() {
    if(_serialPort !=nullptr) {
        _serialPort->close();
        delete _serialPort;
    }
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
        QObject::connect(_serialPort, &QSerialPort::readyRead,this, &SerialPort::dataReady);

    }

    return _serialPort->isOpen();

}

void SerialPort::dataReady() {
    if( _serialPort->isOpen()) {
        emit dataReceived(_serialPort->readAll());
    }
}



