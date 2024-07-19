#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    bool connectToSerial(QString portName, QString baudRate);
    void writeToSerial(QByteArray &data);
    ~SerialPort(); // destructor

private:
    QSerialPort* _serialPort;

signals:
    // void dataReceived(QByteArray(b));
    void dataReceived(QString s);

private slots:
    void dataReady();

};

#endif // SERIALPORT_H
