#include "radio.h"

#include <iostream>
#include <RF24/RF24.h>

using std::cout;
using std::endl;

const uint8_t STATION_ADDRESS[6] = "WTRst";
const uint8_t BASE_ADDRESS[6] = "WTRbs";

#define CE_PIN 25
#define CS_PIN 0

RF24 rad(CE_PIN, CS_PIN);

bool active;

void radio::begin() {
    cout << "> Begin Radio" << endl;
    rad.begin();

    rad.setDataRate(RF24_250KBPS);
    rad.setRetries(15, 15);
    rad.setPayloadSize(PACKET_SIZE);
    
    rad.openWritingPipe(STATION_ADDRESS);
    rad.openReadingPipe(1, BASE_ADDRESS);

    rad.powerDown();
    active = false;

    rad.printDetails();

    cout << "< Begin Radio Complete" << endl;
}

void radio::update() {
    cout << "Update" << endl;
}