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
#include <FS.h>
#include <SPIFFS.h>

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
    RECEIVE_TEST_DATA,
    CONFIRM_TEST_DATA
};

uint8_t current_state = STATE::HANDSHAKE;

/**
 * XMODEM serial function prototypes
 * 
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void InitSPIFFS();
void InitLEDS();
void SwitchLEDs();
void InitXMODEM();
void SerialEvent();
void ParseSerial(char*);

//////////////////// SPIFFS FILE OPERATIONS ///////////////////////////
#define FORMAT_SPIFFS_IF_FAILED 1
const char* test_data_file = "/data.csv";

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);
    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void readTestDataFile() {
    File logFile = SPIFFS.open(test_data_file, "r");
    if (logFile) {
    Serial.println("Log file contents:");
    while (logFile.available()) {
        Serial.write(logFile.read());
    }
    logFile.close();
    } else {
    Serial.println("Failed to open log file for reading.");
    }
}

void InitSPIFFS() {
    if(!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
        Serial.println("SPIFFS mount failed"); // TODO: Set a flag for test GUI
        return;
    } else {
        Serial.println("<SPIFFS init success>");
    }
}

//////////////////// END OF SPIFFS FILE OPERATIONS ///////////////////////////

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

    Serial.println(SOH);

    // call the trasmitter 
    Serial.begin(BAUDRATE);
    Serial.print(NAK);
    Serial.print("\n");
    Serial.flush();
    
}

/*!****************************************************************************
 * @brief Parse the received serial command
 *******************************************************************************/
int value = 0;
void ParseSerial(char* buffer) {

    // parse the int
    

    if(strcmp(buffer, SOH_CHR) == 0) {
    // if(buffer == SOH){
        
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

        value = value*ch + (ch - '0');
        
        // if(serial_index < MAX_CMD_LENGTH && (ch != '\n') ) { // use newline to signal end of command
        //     serial_buffer[serial_index++] = ch;
        // } else {
        //     // here when buffer is full or a newline is received
        //     Serial.println(serial_buffer);
        //     ParseSerial(serial_buffer);
        //     serial_buffer[serial_index] = 0; // terminate the string with a 0
        //     serial_index = 0;
            
        // }
       
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

            // HERE - LOG THE CSV STRING TO EXTERNAL FLASH MEMORY
            //Serial.println(test_data_buffer);
            // open file in append mode
            File data_file = SPIFFS.open(test_data_file, "a");
            if(data_file) {
                data_file.print(test_data_buffer);
                data_file.println(); // start a new line
                data_file.close();
            } else {
                Serial.println("<Failed to write to file>");
            }

        }

    }
}

void setup() {
    Serial.begin(BAUDRATE);
    InitLEDS();
    InitSPIFFS();

    // listDir(SPIFFS, "/", 0);
    // writeFile(SPIFFS, "/test-data.txt", "TEST-DATA\r\n");
    // readFile(SPIFFS, test_data_file);

}

void loop() {

    ////////////////// TEST IF DATA RECEIVED IN DATA FILE /////////////////
    // if(Serial.available() > 0) {
    //     char cmd = Serial.read();
    //     if(cmd == 'R') { // r for read
    //         current_state = STATE::CONFIRM_TEST_DATA;
            
    //     }

    //     if(cmd == EOT) {
    //         Serial.println("END OF TRANSMISSION");
    //     }

    // }

    //////////////////////////////////////////////////////////////////////

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

        // RECEIVE DATA STATE
        case STATE::RECEIVE_TEST_DATA:
            receiveTestDataSerialEvent();
            // Serial.println(test_data_buffer);
            // Serial.println("<RECEIVING_DATA_STATE>");
            break;
        
        // CONFIRM TEST DATA
        case STATE::CONFIRM_TEST_DATA:
            // Serial.println(F("<CONFIRM_SERIAL_STATE>")); //TODO: MOVE to F 
            readTestDataFile();
            break;

        default:
            break;
    }    
    
}