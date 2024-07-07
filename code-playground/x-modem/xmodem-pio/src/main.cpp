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
uint8_t SOH_recvd_flag = 0; /*!< Transmitter acknowledged?  */

unsigned long last_NAK_time = 0;
unsigned long current_NAK_time = 0;
char SOH_CHR[6] = "SOH";

/* define XMODEM commands in HEX */
#define SOH     0x01    /*!< start of header */
#define EOT     0x04    /*!< end of transmission */
#define ACK     0x06    /*!< positive acknowledgement */
#define NAK     0x15    /*!< negative acknowledgement */
#define CAN     0x18    /*!< cancel */

#define MAX_CSV_LENGTH 10 /*!< Maximum length of the csv string that can be received */

char serial_buffer[MAX_CSV_LENGTH];
int16_t serial_index = 0;

uint8_t test_led = 15;

/**
 * XMODEM serial function prototypes
 * 
 */
void InitXMODEM();
void SerialEvent();
void ParseSerial(char*);

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
 * @brief Parse the received serial command
 *******************************************************************************/
void ParseSerial(char* buffer) {

    if(strcmp(buffer, SOH_CHR)) {
        Serial.println("Start of transmission");
        SOH_recvd_flag = 1;
        digitalWrite(test_led, HIGH);
    } else {
        Serial.println("Unknown");
    }

}

/*!****************************************************************************
 * @brief Receive serial message
 *******************************************************************************/
void serialEvent() {
    while (Serial.available()) {
        char ch = Serial.read();
        Serial.write(ch);

        if(serial_index < MAX_CSV_LENGTH && (ch != '\n') ) { // use newline to signal end of command
            serial_buffer[serial_index++] = ch;
        } else {
            // here when buffer is full or a newline is received
            serial_buffer[serial_index] = 0; // terminate the string with a 0
            serial_index = 0;
            ParseSerial(serial_buffer);
        }
       
    }
}

void setup() {
     Serial.begin(BAUDRATE);
     pinMode(test_led, OUTPUT);
     digitalWrite(test_led, LOW);
}

void loop() {

    if(!SOH_recvd_flag) {
        current_NAK_time = millis();
        if( (current_NAK_time - last_NAK_time) > NAK_INTERVAL) {
            // send a NAK command
            // TODO: check is ACK received flag
            InitXMODEM();

            last_NAK_time = current_NAK_time;
        }
    }
    
    
}