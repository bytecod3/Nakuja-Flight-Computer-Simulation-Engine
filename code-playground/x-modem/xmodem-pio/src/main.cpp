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
#include <SD.h>
#include <SPIFFS.h>


uint8_t RUN_MODE = 0;
uint8_t TEST_MODE = 0;

#define BAUDRATE        115200
#define NAK_INTERVAL    4000 /*!< Interval in which to send the NAK command to the transmitter */

//  Flags
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

// pins
// uint8_t soh_ack_led = 15;
uint8_t recv_data_led = 2;          /*!< External flash memory chip select pin */  
uint8_t red_led = 15;               /*!< Red LED pin */
uint8_t green_led = 4;              /*!< Green LED pin */
uint8_t buzzer = 22;                // TODO: change this pin - it is used as SCL
uint8_t SET_TEST_MODE_PIN = 14;     /*!< Pin to set the flight computer to TEST mode */
uint8_t SET_RUN_MODE_PIN = 13;      /*!< Pin to set the flight computer to RUN mode */
uint8_t SD_CS_PIN = 26;             /*!< Chip select pin for SD card */

/*!*****************************************************************************
 * @brief This enum holds the states during flight computer test mode 
 *******************************************************************************/
enum STATE {
    HANDSHAKE = 0,      /*!< state to establish initial communication with transmitter */
    RECEIVE_TEST_DATA,  /*!< sets the flight computer to receive test data over serial */
    CONFIRM_TEST_DATA
};

uint8_t current_state = STATE::HANDSHAKE; /*!< Define current state the flight computer is in */

/**
 * XMODEM serial function prototypes
 * 
 */
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
void InitSPIFFS();
void initSD(); // TODO: return a bool
void InitGPIO();
void SwitchLEDs(uint8_t, uint8_t);
void InitXMODEM();
void SerialEvent();
void ParseSerial(char*);
void checkRunTestToggle();

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
        Serial.println(F("SPIFFS mount failed")); // TODO: Set a flag for test GUI
        return;
    } else {
        Serial.println(F("SPIFFS init success"));
    }
}

void initSD() {
    if(!SD.begin(SD_CS_PIN)) {
        delay(100);
        Serial.println(F("[SD Card mounting failed]"));
        return;
    } else {
        Serial.println(F("[SD card Init OK!"));
    }

    uint8_t cardType = SD.cardType();
    if(cardType == CARD_NONE) {
        Serial.println("[No SD card attached]");
        return;
    }

    // initialize test data file 
    File file = SD.open("/data.csv", FILE_WRITE); // TODO: change file name to const char*

    if(!file) {
        Serial.println("[File does not exist. Creating file]");
        writeFile(SD, "/data.csv", "SSID, SECURITY, CHANNEL, LAT, LONG, TIME \r\n");
    } else {
        Serial.println("[Data file already exists]");
    }

    file.close();
    
}

//////////////////// END OF SPIFFS FILE OPERATIONS ///////////////////////////

/*!****************************************************************************
 * @brief Inititialize the GPIOs
 *******************************************************************************/
void InitGPIO() {
    pinMode(red_led, OUTPUT);
    pinMode(green_led, OUTPUT);
    pinMode(SET_TEST_MODE_PIN, INPUT);
    pinMode(SET_RUN_MODE_PIN, INPUT);

    // set LEDs to a known starting state
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
}

/*!****************************************************************************
 * @brief Switch the LEDS states
 *******************************************************************************/
void SwitchLEDs(uint8_t red_state, uint8_t green_state) {
    digitalWrite(red_led, red_state);
    digitalWrite(green_led, green_state);
}

// non blocking timings
unsigned long last_buzz = 0;
unsigned long current_buzz = 0;
unsigned long buzz_interval = 200;
uint8_t buzzer_state = LOW;

unsigned long last_blink = 0;
unsigned long current_blink = 0;
unsigned long blink_interval = 200;
uint8_t led_state = LOW;

/*!****************************************************************************
 * @brief Buzz the buzzer for a given buzz_interval 
 * This function is non-blocking
 *******************************************************************************/
void buzz() {
    current_buzz = millis();
    if((current_buzz - last_buzz) > buzz_interval) {
        if(buzzer_state == LOW) {
            buzzer_state = HIGH;
        } else {
            buzzer_state = LOW;
        }

        digitalWrite(buzzer, buzzer_state);

        last_buzz = current_buzz;
    }

}

/*!****************************************************************************
 * @brief implements non-blocking blink
 *******************************************************************************/
void blink_200ms(uint8_t led_pin) {
    current_blink = millis();
    if((current_blink - last_blink) > blink_interval) {
        if(led_state == LOW) {
            led_state = HIGH;
        } else if(led_state == HIGH) {
            led_state = LOW;
        }

        digitalWrite(led_pin, led_state);
        last_blink = current_blink;
    }
}

/*!****************************************************************************
 * @brief Sample the RUN/TEST toggle pins to check whether the fligh tcomputer is in test mode 
 * or run mode.
 * If in TEST mode, define the TEST flag
 * If in RUN mode, define the RUN flag
 * TEST_MODE Pin and RUN_MODE pin are both pulled HIGH. When you set the jumper, you pull that pin to 
 * LOW.    
 *******************************************************************************/
void checkRunTestToggle() {

    if( (digitalRead(SET_RUN_MODE_PIN) == 0) && (digitalRead(SET_TEST_MODE_PIN) == 1) ) {
        // run mode
        RUN_MODE = 1;
        TEST_MODE = 0;
        SwitchLEDs(TEST_MODE, RUN_MODE);
    }
    
    if((digitalRead(SET_RUN_MODE_PIN) == 1) && (digitalRead(SET_TEST_MODE_PIN) == 0)){
        // test mode
        RUN_MODE = 0;
        TEST_MODE = 1;

        SwitchLEDs(TEST_MODE, RUN_MODE);
    }

    // here the jumper has been removed. we are neither in the TEST or RUN mode
    // INVALID STATE 
    if((digitalRead(SET_RUN_MODE_PIN) == 1) && (digitalRead(SET_TEST_MODE_PIN) == 1)) {
        TEST_MODE = 0;
        RUN_MODE = 0;
        SwitchLEDs(!TEST_MODE, !RUN_MODE);
    }

    // Serial.print("RUN MODE PIN: "); Serial.print(digitalRead(SET_RUN_MODE_PIN));
    // Serial.print(", TEST MODE PIN: "); Serial.print(digitalRead(SET_TEST_MODE_PIN));
    // Serial.println();

}



/**
 * XMODEM serial function definition
 */

/*!****************************************************************************
 * @brief Initiate XMODEM protocol by sending a NAK command every 4 seconds until the transmitter returns an ACK signal
 * @param none
 *******************************************************************************/
void InitXMODEM() {

    // call the trasmitter 
    Serial.begin(BAUDRATE);
    Serial.print(NAK); 
    Serial.print("\n");
    Serial.flush();
    
}

/*!****************************************************************************
 * @brief Parse the received serial command if it is a string 
 *******************************************************************************/
int value = 0;
void ParseSerialBuffer(char* buffer) {

    if(strcmp(buffer, SOH_CHR) == 0) {
    // if(buffer == SOH){
        
        Serial.println(F("<Start of transmission>"));
        SOH_recvd_flag = 1;
        digitalWrite(red_led, HIGH);
        Serial.println(F("<SOH rcvd from receiver> Waiting for data..."));

        // put the MCU in data receive state 
        current_state = STATE::RECEIVE_TEST_DATA;
        SwitchLEDs(0, 1);

    } else {
        Serial.println("Unknown");
    }

}

/*!****************************************************************************
 * @brief Parse the received serial command if it is a digit
 We are only interested in numeric values being sent by the transmitter to us, the receiver
 *******************************************************************************/
void ParseSerialNumeric(int value) {
    Serial.print("Receive val: ");
    Serial.println(value);

    if(value == 1) // SOH: numeric 1
    {        
        Serial.println("<Start of transmission>");
        SOH_recvd_flag = 1;
        Serial.println("<SOH rcvd> Waiting for data");

        // put the MCU in data receive state
        // any serial data after this will be the actual test data being received
        SwitchLEDs(0, 1); // red off, green on
        current_state = STATE::RECEIVE_TEST_DATA;

    } else if(value == 4) {
        // EOT: numeric 4 
        Serial.println("Unknown");
    }
}

/*!****************************************************************************
 * @brief Receive serial message during handshake
 *******************************************************************************/
void handshakeSerialEvent() {
    SwitchLEDs(1,0);
    while (Serial.available()) {
        char ch = Serial.read();

        if(isDigit(ch)) { // character between 0 an 9
            // accumulate value
            value = value*ch + (ch - '0');
        } else if (ch == '\n') {
            Serial.print("SerEvent: ");
            Serial.println(value);
            ParseSerialNumeric(value);
            value = 0; // reset value for the next transmission burst
        }

        
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
    InitGPIO();
    pinMode(buzzer, OUTPUT);

    // checkRunTestToggle();

    initSD();
    InitSPIFFS();

    // reset the system log file
    system_logger.logToFile(SPIFFS, 1, client, level, system_log_file, "readData Function call OK!");

    // listDir(SPIFFS, "/", 0);
    // writeFile(SPIFFS, "/test-data.txt", "TEST-DATA\r\n");
    // readFile(SPIFFS, test_data_file);

}

void loop() {

    // check whether we are in the RUN or TEST mode
    checkRunTestToggle();

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
    if(TEST_MODE) {
        // Serial.println(F("[TEST MODE]"));
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
                // blink_200ms(green_led);
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

    } // end of TEST mode 

    else if(RUN_MODE)  {
        // Serial.println(F("[RUN MODE]"));
    } else if((RUN_MODE == 0) && (TEST_MODE) == 0 ) {
        Serial.println(F("[INVALID MODE]"));
    }

    // start of RUN mode
    
}