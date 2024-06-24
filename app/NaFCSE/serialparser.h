#ifndef SERIALPARSER_H
#define SERIALPARSER_H

#include <QObject>
#include <QVector>

#define DEBUG_STATES 0 // set this to 1 to pring the state names

class SerialParser : public QObject
{
    Q_OBJECT
public:
    explicit SerialParser(QObject *parent = nullptr);

    QVector<QString> packet_vector; // store parsed csv packet
    quint8 flight_state;

    void parseAll(const QString data);
    void decodeStates(const QString s);
    quint8 getCurrentState();
    void testParse();


signals:
};

#endif // SERIALPARSER_H
