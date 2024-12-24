const int drogue = 25;
const int mainc = 12;

int systemcheck = 43;
int del = 200;

void xTaskSendState(void* pvParameters) {
  while(1) {
    Serial.println("PREFLIGHT");
    delay(del);
    Serial.println("POWERED");
    delay(del);
    Serial.println("COASTING");
    delay(del);
    Serial.println("APOGEE");
    delay(del);
    Serial.println("BALLISTIC");
    delay(del);
    Serial.println("DROGUE");
    delay(del);
    Serial.println("DROGUE_DESC");
    delay(del);
    Serial.println("MAIN");
    delay(del);
    Serial.println("MAIN_DESC");
    delay(1000);
    Serial.println("POST_FLIGHT");
    delay(del);
    Serial.println("END");
    delay(del);
  }

}

void checkSystem() {
  Serial.println(systemcheck);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(drogue, OUTPUT);
  pinMode(mainc, OUTPUT);

  xTaskCreate(xTaskSendState, "sendState", 1024, NULL, 1, NULL);

}

void loop() {
//  digitalWrite(drogue, HIGH);
//  delay(1000);
//  digitalWrite(drogue, LOW);
//  delay(1000);
//
//  delay(1000);
//
//  digitalWrite(mainc, HIGH);
//  delay(1000);
//  digitalWrite(mainc, LOW);
//  delay(1000);

  if(Serial.available()) {
    char ch = Serial.read();
    if(ch >= '0' && ch <= '9') { 
      if (ch == '1') checkSystem(); 
    }
  }

}
