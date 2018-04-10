#include "radio.h"

#include <RF24/RF24.h>

const byte STATION_ADDRESS[6] = "WTRst";
const byte BASE_ADDRESS[6] = "WTRbs";

#define CE_PIN 25
#define CS_PIN 0

RF24 radio(CE_PIN, CS_PIN);

bool active;

void radio::begin() {
    radio.begin();

    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);
    radio.setPayloadSize(PACKET_SIZE);
    
    radio.openWritingPipe(STATION_ADDRESS);
    radio.openReadingPipe(1, BASE_ADDRESS);

    radio.powerDown();
    active = false;
}