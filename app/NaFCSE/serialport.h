#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    bool connectToSerial(QString portName, QString baudRate);

signals:

private:
    QSerialPort* _serialPort;


};

#endif // SERIALPORT_H
