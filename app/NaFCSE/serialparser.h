#ifndef SERIALPARSER_H
#define SERIALPARSER_H

#include <QObject>

class SerialParser : public QObject
{
    Q_OBJECT
public:
    explicit SerialParser(QObject *parent = nullptr);
    void parseAll(QByteArray data);
    void decodeStates();
    void testParse();

signals:
};

#endif // SERIALPARSER_H
