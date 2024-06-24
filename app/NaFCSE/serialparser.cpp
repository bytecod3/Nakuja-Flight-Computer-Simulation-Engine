#include "serialparser.h"
#include <QVector>
#include <QDebug>
#include <sstream>
#include <string>

SerialParser::SerialParser(QObject *parent)
    : QObject{parent}
{}

/**
 * @brief SerialParser::testParse
 * A test function
 */
void SerialParser::testParse() {
    qDebug() << "Parser called";
}

/**
 * @brief SerialParser::parseAll
 * @param data
 * separate serial data into individual variables that represent the telemetry packet
 *
 * TODO: add a mechanism to make the data points variable
 */
void SerialParser::parseAll(const QString data) {
    // sanitize data
    QString clean_data = data.trimmed();

    QStringList telemetry_packet = clean_data.split(',');
    QString recordNumber = telemetry_packet.at(0);

    qDebug() << clean_data;

}

void decodeStates() {

}
