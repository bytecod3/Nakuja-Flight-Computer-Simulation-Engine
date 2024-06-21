/**
 * This file sends the state regularly
 */

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

#define NUM_OF_STATES 10

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
unsigned long long current_time, previous_time = 0;
uint16_t burst_interval = 1000; // send after every burst_interval

void setup() {
  Serial.begin(115200);

}

void loop() {

  current_time = millis();
  if(current_time - previous_time >= burst_interval){
    previous_time = current_time;
    current_state = states_arr[counter];
    counter++;

    // reset counter 
    if(counter > NUM_OF_STATES-1) {
      counter = 0;
    }

    Serial.println(current_state);
  }

}
