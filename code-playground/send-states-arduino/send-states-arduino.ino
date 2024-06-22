/**
 * This program emulates a telemetry packet and sends it to the serial monitor
 */

#define NUM_OF_STATES 10
char telemetry_packet[100];

enum states {
  PRE_FLIGHT = 0,
  POWERED_FLIGHT,
  COASTING,
  APOGEE,
  BALLISTIC_DESCENT,
  DROGUE_CHUTE_EJECT,
  DROGUE_CHUTE_DESCENT,
  MAIN_CUTE_EJECT,
  MAIN_CHUTE_DESCENT,
  POST_FLIGHT
};

// an array to hold the states 
uint8_t states_arr[NUM_OF_STATES] = {
  states::PRE_FLIGHT,
  states::POWERED_FLIGHT,
  states::COASTING,
  states::APOGEE,
  states::BALLISTIC_DESCENT,
  states::DROGUE_CHUTE_EJECT,
  states::DROGUE_CHUTE_DESCENT,
  states::MAIN_CUTE_EJECT,
  states::MAIN_CHUTE_DESCENT,
  states::POST_FLIGHT
};

uint8_t current_state = states::PRE_FLIGHT;
uint8_t counter = 0;
uint8_t state_counter = 0;
unsigned long long current_time, previous_time = 0;
uint16_t burst_interval = 1000; // send after every burst_interval

void setup() {
  Serial.begin(115200);

  // seed the random number generator with pin 36
  randomSeed(analogRead(36));
}

void loop() {
  current_time = millis();
  
  if(current_time - previous_time >= burst_interval){
    previous_time = current_time;
    state_counter++;
    counter++;
    
    // record number 
    uint8_t record_number = counter;
    
    // state
    current_state = states_arr[state_counter];
    
    // operation mode
    uint8_t operation_mode = 0; 
    
    // x acceleration
    float ax = generateFakeFloat(0.3);

    // y acceleration 
    float ay = generateFakeFloat(0.4);

    // z acceleration 
    float az = generateFakeFloat(0.4);

    // pitch
    float pitch = generateFakeFloat(0.4);

    // roll
    float roll = generateFakeFloat(0.4);

    // gx 
    float gx = generateFakeFloat(0.1);

    // gy
    float gy = generateFakeFloat(0.2);

    // gz
    float gz = generateFakeFloat(0.9);

    // latitude amd longitude
    double latitude = -1.05;
    double longitude = 37.10;

    // gps altitude 
    uint16_t gps_altitude = 1520;

    // pressure 
    float pressure = generateFakeFloat(0.2);

    // temperature 
    uint8_t temperature = 23;

    // altitude agl
    uint16_t altitude_agl = 1400;

    // velocity
    float velocity = generateFakeFloat(0.6);

    // package the packet 
    sprintf(telemetry_packet, 
        "%d,%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%.2f,%d,%d,%.2f",
        record_number,
        current_state,
        operation_mode,
        ax, ay, az,
        pitch, 
        roll,
        gx, gy, gz, 
        latitude, longitude,
        gps_altitude,
        pressure,
        temperature,
        altitude_agl,
        velocity
      );
    
    // reset counter 
    if(state_counter > NUM_OF_STATES-1) {
      state_counter = 0;
    }

    Serial.println(telemetry_packet);
  }

}

// generates fake float
float generateFakeFloat(float x){
  return random(0, 10) + x;
}
