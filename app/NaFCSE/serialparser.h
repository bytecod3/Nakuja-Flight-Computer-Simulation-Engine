#ifndef SERIALPARSER_H
#define SERIALPARSER_H

#include <QObject>
#include <QVector>

class SerialParser : public QObject
{
    Q_OBJECT
public:
    explicit SerialParser(QObject *parent = nullptr);

    QVector<QString> packet_vector; // store parsed csv packet

    void parseAll(const QByteArray data);
    void decodeStates();
    void testParse();


signals:
};

#endif // SERIALPARSER_H
