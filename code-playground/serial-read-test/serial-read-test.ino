
int i = 0;
char buffer[20];
int numeric_value;

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

void setup() {
  
  Serial.begin(115200);
}

void loop() {
  while(Serial.available()) {
    char c = Serial.read();
    if(c >= '0' && c <= '9' ){ // between 0 and 9
      numeric_value = (numeric_value*10) + (c - '0');
    } else if (c == 10) { // newline character
      Serial.println(numeric_value);

      parseCmd(numeric_value);
      numeric_value = 0;
    }

    

    delay(10);
  }

  

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
