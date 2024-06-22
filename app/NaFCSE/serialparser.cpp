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
void SerialParser::parseAll(const QByteArray data) {
    // sanitize data
    std::string clean_data = data.trimmed();

    std::stringstream ss(clean_data);
    // for(int i; ss >> i;){
    //     packet_vector.push_back(i);
    //     if(ss.peek() == ',') { // skip ',' delimiter
    //         ss.ignore();
    //     }
    // }

    while (ss.good()) {
        std::string substr;
        getline(ss, substr, ",");
        packet_vector.push_back(substr);
    }

    // qDebug() << clean_data;

    // debug splitted packet
    for(std::size_t i=0; i < packet_vector.size(); i++) {
        qDebug() << packet_vector[i];
    }

}

void decodeStates() {

}
