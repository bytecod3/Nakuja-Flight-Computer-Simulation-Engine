
#ifndef BASIC_STATE_MACHINE_USER_EVENT_H
#define BASIC_STATE_MACHINE_USER_EVENT_H

#include <Arduino.h>

typedef struct {
    char msg[10];
    uint8_t counter;
} USER_EVENT_T;

#endif
