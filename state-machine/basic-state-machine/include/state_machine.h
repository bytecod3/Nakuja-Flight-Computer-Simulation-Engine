//
// Created by USER on 5/15/2024.
//

#ifndef BASIC_STATE_MACHINE_STATE_MACHINE_H
#define BASIC_STATE_MACHINE_STATE_MACHINE_H

typedef enum{
    PRE_FLIGHT = 1,
    POWERED_FLIGHT,
    COASTING,
    APOGEE,
    CHUTE_EJECTION,
    POST_FLIGHT
} STATE;

// states as strings - solely for debugging - DO NOT transmit these
char pre_flight[20] = "PREFLIGHT";
char powered_flight[20] = "POWERED_FLIGHT";
char coasting[20] = "COASTING";
char apogee[20] = "APOGEE";
char chute_ejection[20] = "CHUTE_EJECTION";
char post_flight[20] ="POST_FLIGHT";

#endif //BASIC_STATE_MACHINE_STATE_MACHINE_H
