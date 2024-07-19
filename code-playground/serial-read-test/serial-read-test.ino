#include <SD.h>
#include <FS.h>

int i = 0;
char buffer[20];
int numeric_value;
uint8_t SD_CS_PIN = 26;             /*!< Chip select pin for SD card */
int green_led = 4;

// timing variable for the leds
unsigned long last_blink = 0;
unsigned long current_blink = 0;
unsigned long blink_interval = 200;
uint8_t led_state = LOW;

// timing variable for the buzzer
unsigned long last_buzz = 0;
unsigned long current_buzz = 0;
unsigned long buzz_interval = 200;
uint8_t buzzer_state = LOW;
int buzzer = 22;

void buzz_200ms() {
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


void parseCmd(int v) {
  // parse
  switch(v){
    case 1:
      Serial.println("SOH");
      break;

    case 4:
      Serial.println("EOT");
      break;

    case 21:
      Serial.println("NAK");
      break;

    default:
      Serial.println(F("INVALID"));
  }

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

void initSD() {
    if(!SD.begin(SD_CS_PIN)) {
        Serial.println(F("[SD Card mounting failed]"));
        return;
    } else {
      Serial.println("[SD card Init OK!");
    }

    uint8_t cardType = SD.cardType();
    
    if(cardType == CARD_NONE) {
        Serial.println("[No SD card attached]");
        return;
    } else {
      Serial.println(cardType);
    }

    // initialize test data file 
    File file = SD.open("/data.csv", FILE_WRITE);

    if(!file) {
        Serial.println("[File does not exist. Creating file]");
        writeFile(SD, "/data.csv", "TESt data headers \r\n");
    } else {
        Serial.println("[Data file already exists]");
    }

    file.close();
    
}

void setup() {
  
  Serial.begin(115200);
  // initSD();
  pinMode(green_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
}

void loop() {

  // blink_200ms(green_led);
  buzz_200ms();

  // while(Serial.available()) {
  //   char c = Serial.read();
  //   if(c >= '0' && c <= '9' ){ // between 0 and 9
  //     numeric_value = (numeric_value*10) + (c - '0');
  //   } else if (c == 10) { // newline character
  //     Serial.println(numeric_value);

  //     parseCmd(numeric_value);
  //     numeric_value = 0;
  //   }

  //   delay(10);
  // }

  // while(Serial.available() > 0) {
  //   char ch = Serial.read();

  //   if (i<20 && ch != '\n') {
  //     buffer[i++] = ch;
  //   } else {
  //     buffer[i] = '\n';
  //     i = 0;
  //   }

  //   delay(10); // allow serial buffer to receive
  // }

  // Serial.println(buffer);
}
