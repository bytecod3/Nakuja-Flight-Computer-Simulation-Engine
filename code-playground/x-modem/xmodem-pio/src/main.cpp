/**
 * 
 * 
 * Author: Edwin Mwiti, Friday July 5th 2024
 * 
 */

/*!****************************************************************************
 * @file main.cpp
 * @brief This file implements a subset of the XMODEM protocol
 * This program implements the XMODEM protocol version with no error detection and/or correction * 
 * 
 *******************************************************************************/

#include <Arduino.h>
#define BAUDRATE 115200

/* define XMODEM commands in HEX */
#define SOH     0x01    /*!< start of header */
#define EOT     0x04    /*!< end of transmission */
#define ACK     0x06    /*!< positive acknowledgement */
#define NAK     0x15    /*!< negative acknowledgement */
#define CAN     0x18    /*!< cancel */

/**
 * XMODEM serial function prototypes
 * 
 */
void InitXMODEM();
void SerialEvent();
void ParseSerial(char* serialString);

/**
 * XMODEM serial function definition
 * 
 */

/*!****************************************************************************
 * @brief Initiate XMODEM protocol by sends a NAK command every 4 seconds until the transmitter returns an ACK signal
 * @param none
 *******************************************************************************/
void InitXMODEM() {

}

/**
 * Receive and process a serial commmand
 */
void SerialEvent() {
    while (Serial.available()) {

    }

}


void setup() {
    Serial.begin(BAUDRATE);
    
}

void loop() {

}