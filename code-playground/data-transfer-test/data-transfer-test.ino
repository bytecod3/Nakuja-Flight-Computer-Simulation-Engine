char data_buffer[256];
int data_buffer_index = 0;

void recvData();

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Serial ready");

}

void loop() {
  
  recvData();
  if( (sizeof(data_buffer)/sizeof(char)) != 0 ) {
    Serial.println(data_buffer);
  }
  
}

void recvData() {
  if(Serial.available()) {
    char ch = Serial.read();

    if(ch != '\n') {
      data_buffer[data_buffer_index++] = ch;
    } else {
      data_buffer[data_buffer_index] = 0; // null terminator
      data_buffer_index = 0;
    }

  }

}
