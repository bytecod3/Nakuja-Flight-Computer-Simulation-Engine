
#include "serialport.h"
#include <QDebug>

SerialPort::SerialPort(QObject *parent)
    : QObject{parent},
    _serialPort(nullptr)
{}

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

/**
 * @brief SerialPort::dataReady
 * process data received from serial port
 */
void SerialPort::dataReady() {
    QString serial_buffer;

    if(_serialPort->isOpen()) {

        if(_serialPort->canReadLine()) {
            QString data = _serialPort->readAll();
            serial_buffer.append(data);

            // wait for complete data
            if(!data.contains("\n")) {
                serial_buffer.append(data);
            } else {
                serial_buffer.clear();
                emit dataReceived(data);
            }

        }

    }
}



