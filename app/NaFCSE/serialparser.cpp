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

    // split the received packet into csv variables
    QStringList telemetry_packet = clean_data.split(',');

    // record number
    QString recordNumber = telemetry_packet.at(0);

    // flight state
    QString state = telemetry_packet.at(1); // see telemetry packet structure

    this->flight_state = state.toUInt();

    qDebug() << this->flight_state;

    // this->decodeStates(state);

}

/**
 * @brief decodeStates
 * @param s
 *
 * Handle the states received from the serial monitor
 *
 */
void SerialParser::decodeStates(const QString s) {
    quint8 state = s.toInt();

// print states on console
#if DEBUG_STATES
    switch (state) {
    case 0:
        qDebug() << "PREFLIGHT";
        break;

    case 1:
        qDebug() << "POWERED FLIGHT";
        break;

    case 2:
        qDebug() << "COASTING";
        break;

    case 3:
        qDebug() << "APOGEE";
        break;

    case 4:
        qDebug() << "DROGUE-CHUTE-DEPLOY";
        break;

    case 5:
        qDebug() << "DROGUE-CHUTE-DESCENT";
        break;

    case 6:
        qDebug() << "MAIN-CHUTE-DEPLOY";
        break;

    case 7:
        qDebug() << "MAIN-CHUTE-DESCENT";
        break;

    case 8:
        qDebug() << "POST-FLIGHT";
        break;

    default:
        break;
    }
#endif

}

/**
 * @brief SerialParser::getCurrentState
 * @return return the current state
 *
 */
quint8 SerialParser::getCurrentState() {
    return this->flight_state;
}
