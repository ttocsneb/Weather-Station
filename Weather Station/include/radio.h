#ifndef __RADIO__
#define __RADIO__

#include "main.h"
#include <RF24.h>

#define PACKET_SIZE 20
#define RETRIES 4

// Commands

#define COMMAND_GET_WEATHER 0xA0
#define COMMAND_GET_SETTINGS 0xA5
#define COMMAND_GET_STATUS 0xAB

#define COMMAND_SET_SETTINGS 0xD0

#define COMMAND_RESET 0xB0
#define COMMAND_RESET_STATUS 0xB3

#define COMMAND_RESET_SETTINGS 0xC0
#define COMMAND_RESTART 0xCA


namespace radio {
    extern RF24 radio;

    extern uint16_t lostPackets;

    void begin();

    void update();
}

#endif