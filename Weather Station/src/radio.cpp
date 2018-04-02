#include "radio.h"

const byte STATION_ADDRESS[6] = "WTRst";
const byte BASE_ADDRESS[6] = "WTRbs";

void radio::begin() {
    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);

    radio.openWritingPipe(BASE_ADDRESS);
    radio.openReadingPipe(1, STATION_ADDRESS);

    radio.powerDown();
}