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

#define MAX_CMD_LENGTH 10 /*!< Maximum length of the XMODEM command string that can be received */
#define MAX_CSV_LENGTH 256 /*!< Maximum length of the csv string that can be received */

// buffer to store the XMODEM commands 
char serial_buffer[MAX_CMD_LENGTH];
int16_t serial_index = 0;

// buffer to store the CSV test data
char test_data_buffer[MAX_CSV_LENGTH]; 
int16_t test_data_serial_index = 0;

uint8_t soh_ack_led = 15;
uint8_t recv_data_led = 2;

// states 
enum STATE {
    HANDSHAKE = 0,
    RECEIVE_TEST_DATA
};

uint8_t current_state = STATE::HANDSHAKE;

/**
 * XMODEM serial function prototypes
 * 
 */
void InitLEDS();
void SwitchLEDs();
void InitXMODEM();
void SerialEvent();
void ParseSerial(char*);

/*!****************************************************************************
 * @brief Inititlaize the LED GPIOs
 *******************************************************************************/
void InitLEDS() {
    pinMode(soh_ack_led, OUTPUT);
    pinMode(recv_data_led, OUTPUT);

    // set LEDs to a known starting state
    digitalWrite(soh_ack_led, LOW);
    digitalWrite(recv_data_led, LOW);
}

/*!****************************************************************************
 * @brief Switch the LEDS states
 *******************************************************************************/
void SwitchLEDs() {
    digitalWrite(soh_ack_led, LOW);
    digitalWrite(recv_data_led, HIGH);
}

/**
 * XMODEM serial function definition
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
        Serial.println("<Start of transmission>");
        SOH_recvd_flag = 1;
        digitalWrite(soh_ack_led, HIGH);
        Serial.println("<SOH rcvd> Waiting for data");

        // put the MCU in data receive state 
        current_state = STATE::RECEIVE_TEST_DATA;
        SwitchLEDs();

    } else {
        Serial.println("Unknown");
    }

}

/*!****************************************************************************
 * @brief Receive serial message during handshake
 *******************************************************************************/
void handshakeSerialEvent() {
    while (Serial.available()) {
        char ch = Serial.read();
        Serial.write(ch);

        if(serial_index < MAX_CMD_LENGTH && (ch != '\n') ) { // use newline to signal end of command
            serial_buffer[serial_index++] = ch;
        } else {
            // here when buffer is full or a newline is received
            serial_buffer[serial_index] = 0; // terminate the string with a 0
            serial_index = 0;
            ParseSerial(serial_buffer);
        }
       
    }
}

/*!****************************************************************************
 * @brief Receive serial message during RECEIVE_TEST_DATA state
 * Data received in this state is the actual test data. It is saved into the test flash memory 
 * 
 *******************************************************************************/
void receiveTestDataSerialEvent() {
    while(Serial.available()) {
        char ch = Serial.read();
        Serial.write(ch);

        // each CSV string ends with a newline 
        if(test_data_serial_index < MAX_CSV_LENGTH && (ch != '\n') ) {
            test_data_buffer[test_data_serial_index++] = ch;
        } else {
            // buffer is full or newline is received
            test_data_buffer[test_data_serial_index] = 0; // NUL terminator
            test_data_serial_index = 0;

            // HERE - LOG THE CSV STRING TO THE FLASH MEMORY
            //Serial.println(test_data_buffer);

        }

    }
}

void setup() {
     Serial.begin(BAUDRATE);
     InitLEDS();
}

void loop() {

    switch (current_state) {
        // HANDSHAKE STATE
        case STATE::HANDSHAKE:
            handshakeSerialEvent();
            if(!SOH_recvd_flag) {
                    current_NAK_time = millis();
                    if( (current_NAK_time - last_NAK_time) > NAK_INTERVAL) {
                        // send a NAK command
                        // TODO: check is ACK received flag
                        InitXMODEM();

                        last_NAK_time = current_NAK_time;
                    }
                }  
            break;

        // RECEIVE_DATA_STATE
        case STATE::RECEIVE_TEST_DATA:
            receiveTestDataSerialEvent();
            Serial.println(test_data_buffer);
            // Serial.println("<RECEIVING_DATA_STATE>");
            break;
        
        default:
            break;
    }

    // check the current state
    
         
    
}