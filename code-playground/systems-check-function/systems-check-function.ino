#define CHECK_MASK 0b00000001
#define BIT6_MASK 0b01000000

uint8_t SYSTEM_STATE = 0b01101011;

void setup() {
  Serial.begin(115200);
  Serial.println(SYSTEM_STATE); // 107

}

void loop() {
  // set
  SYSTEM_STATE |= BIT6_MASK;
  Serial.println(SYSTEM_STATE);
  delay(500);
  
  // clear
  SYSTEM_STATE &= ~(BIT6_MASK);
  Serial.println(SYSTEM_STATE);

  delay(500);
  
  

}
