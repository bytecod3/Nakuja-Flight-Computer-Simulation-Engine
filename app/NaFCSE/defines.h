#ifndef DEFINES_H
#define DEFINES_H
#include <QtTypes>

/*!< This enum contains states that the app can be in at any given time  */
enum APP_STATES {
    HANDSHAKE = 0,  /*!< handshake state - here we are trying to establish hardware connections with flight computer */
    NOMINAL,         /*!< nominal state - we have already established connection with flight computer  */
    RUNNING,     /*!< running state - the flight computer has received test data and the simulation is in progress */
    SYSTEM_CHECK
};

extern int current_app_state;


#endif // DEFINES_H
