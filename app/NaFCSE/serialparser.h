#ifndef SERIALPARSER_H
#define SERIALPARSER_H

#include <QObject>
#include <QVector>

#define DEBUG_STATES 0 // set this to 1 to print the state names

// default state is the HANDSHAKE state, so that we initialize XMODEM communication with the hardware
// the hardware will send a NAK code to us, if we receive this code, we respond with a SOH then
// switch the state to NOMINAL and proceed
// uint8_t current_app_state = APP_STATES::HANDSHAKE;

class SerialParser : public QObject
{
    Q_OBJECT
public:
    explicit SerialParser(QObject *parent = nullptr);

    QVector<QString> packet_vector; // store parsed csv packet
    quint8 flight_state;

    void parseAll(const QString data);
    void decodeStates(const QString s);
    quint8 getCurrentFlightState();
    void testParse();

signals:
};

#endif // SERIALPARSER_H
