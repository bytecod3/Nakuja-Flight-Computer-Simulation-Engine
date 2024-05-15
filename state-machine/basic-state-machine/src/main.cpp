#include <Arduino.h>
#include "user_event.h"
#include "state_machine.h"
#include "string.h"

QueueHandle_t evntQueue = NULL;
TimerHandle_t tHandle = NULL;

/**
 * the first state is PRE-FLIGHT
 * any time the flight computer is restarted, this defaults to PRE-FLIGHT
 * future consideration: restarting during flight should not set the state to PRE-FLIGHT
 * implement a mechanism to remember exactly where it was - perhaps using Non-Volatile-Storage on ESP
 *
 */
uint8_t current_state = STATE::PRE_FLIGHT;

/*
 * prints the current state as a string
 */
char* state_to_str(uint8_t state) {
    switch (state) {
        case 1:
            // preflight
            return pre_flight;
        case 2:
            // powered-flight
            return powered_flight;
        case 3:
            // coasting
            return coasting;
        case 4:
            // apogee
            return apogee;
        case 5:
            // chute ejection
            return chute_ejection;
        case 6:
            // post flight
            return post_flight;
        default:
            return pre_flight;
    }
}

//void tA(void* arg) {
//
//    // initialize struct
//    USER_EVENT_T  data = {"", 0};
//    while(true) {
//        printf("TA task. \r\n");
//
//        // wait for sth to arrive in queue
//        xQueueReceive(evntQueue, &data, 10);
//
//        // call the state machine with this new data
//
//    }
//}

/*
 * This function will execute if the timer expires
 */
void timerCallback(TimerHandle_t xTimerHandle) {
    static uint8_t cnt = 0;
    cnt++;

    USER_EVENT_T data = {"timeout", cnt};

    xQueueSend(evntQueue, &data, 0U);
}

/*
 * change state based on what is received from evntQueue
 */
static void stateMachine(void* arg) {
    // store received data
    USER_EVENT_T rcvd_data;

    while(true) {

        xQueueReceive(evntQueue, &rcvd_data, 10);

        printf("cnt: %d\n", rcvd_data.counter);

        // check the timer value
        if(rcvd_data.counter <= 5) {
            current_state = STATE::PRE_FLIGHT;
        } else if (rcvd_data.counter > 5 && rcvd_data.counter <= 10 ) {
            current_state = STATE::POWERED_FLIGHT;
        } else if (rcvd_data.counter > 10 && rcvd_data.counter <= 15 ) {
            current_state = STATE::COASTING;
        } else if (rcvd_data.counter > 15 && rcvd_data.counter <= 20) {
            current_state = STATE::APOGEE;
        }

        // debug the state
        printf("%s\n", state_to_str(current_state));

        // transition the state based on the timer value
        switch (current_state) {
            case 1:
                // call function to operate the state machine
                // printf("%s\n", state_to_str(current_state));
                break;
            case 5:

                break;
            case 10:

                break;
            default:
                // default state is pre-flight
                // TODO: add a SAFE_MODE check here

                break;
        }

    }
}

void setup() {
    Serial.begin(115200);

    int app_cpu = xPortGetCoreID();

    // create the task
    //TODO: check for successful task creation with task handles
    xTaskCreate(stateMachine, "stateMachine", 2048, NULL, tskIDLE_PRIORITY+1, NULL);

    // create the queue
    evntQueue = xQueueCreate(32, sizeof(USER_EVENT_T));

    tHandle = xTimerCreate(
            "Timer",                // name for debugging
            1000,               // ticks -> 1s
            true,                    // true - reptitive
            NULL,                      // timer's ID is not used
            timerCallback       // function to execute when the timer expires
            );

    // start the timer
    xTimerStart(tHandle, 0);

}

void loop() {
    vTaskDelete(nullptr);
}