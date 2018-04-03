#ifndef __RADIO__
#define __RADIO__

#include "main.h"
#include <RF24.h>

#define PACKET_SIZE 32
#define RETRIES 4
#define LISTEN_TIME 1000

namespace radio {
extern RF24 radio;

extern uint16_t lostPackets;

void begin();

void update();
}

#endif