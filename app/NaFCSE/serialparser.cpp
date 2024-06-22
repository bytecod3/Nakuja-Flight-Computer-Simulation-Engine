#include "serialparser.h"
#include <QDebug>

SerialParser::SerialParser(QObject *parent)
    : QObject{parent}
{}


void SerialParser::testParse() {
    qDebug() << "Parser called";
}
