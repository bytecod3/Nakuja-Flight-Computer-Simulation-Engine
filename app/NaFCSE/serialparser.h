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

    void parseAll(const QString data);
    void decodeStates(const QString s);
    void testParse();


signals:
};

#endif // SERIALPARSER_H
