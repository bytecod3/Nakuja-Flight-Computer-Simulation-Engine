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
#define BAUDRATE        115200
#define NAK_INTERVAL    4000 /*!< Interval in which to send the NAK command to the transmitter */

unsigned long last_NAK_time = 0;
unsigned long current_NAK_time = 0;

/* define XMODEM commands in HEX */
#define SOH     "0x01"    /*!< start of header */
#define EOT     "0x04"    /*!< end of transmission */
#define ACK     "0x06"    /*!< positive acknowledgement */
#define NAK     "0x15"    /*!< negative acknowledgement */
#define CAN     "0x18"    /*!< cancel */

#define MAX_CSV_LENGTH 10 /*!< Maximum length of the csv string that can be received */

char serial_buffer[MAX_CSV_LENGTH];
int16_t serial_index = 0;

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
 * @brief Initiate XMODEM protocol by sending a NAK command every 4 seconds until the transmitter returns an ACK signal
 * @param none
 *******************************************************************************/
void InitXMODEM() {

    // check if the transmitter has acknowledged our call
    Serial.begin(BAUDRATE);
    Serial.print(NAK);
    Serial.print("\n");
    Serial.flush();
    
}


/*!****************************************************************************
 * @brief Receive and process a serial command
 *******************************************************************************/
void SerialEvent() {
    while (Serial.available() > 0) {
        char ch = Serial.read();

        if(serial_index < MAX_CSV_LENGTH) {
            serial_buffer[serial_index++] = ch;
        } else {
            serial_buffer[serial_index] = 0;
        }

        if(ch == '\n') {
            serial_index = 0;
            Serial.println(serial_buffer);
        }
        
    }

}

void setup() {
     Serial.begin(BAUDRATE);
}

void loop() {
    SerialEvent();

    current_NAK_time = millis();
    if( (current_NAK_time - last_NAK_time) > NAK_INTERVAL) {
        // send a NAK command
        // TODO: check is ACK received flag
        InitXMODEM();

        last_NAK_time = current_NAK_time;
    }
    
}