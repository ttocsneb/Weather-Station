#ifndef __RADIO__
#define __RADIO__

#include "main.h"
#include <RF24.h>

#define PACKET_SIZE 32
#define RETRIES 4
#define LISTEN_TIME 1000

#define EOT '\0'

#define COMMAND_SET_VALUE 'S'
#define COMMAND_SET_EEPROM 'E'
#define COMMAND_GET_VALUE 'G'
#define COMMAND_GET_STATUS 'U'
#define COMMAND_LOAD_EEPROM 'L'
#define COMMAND_RESET 'R'

namespace radio {
extern RF24 radio;

extern uint16_t lostPackets;

void begin();

void update();
}

#endif