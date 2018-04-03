#include "main.h"
#include "sensors.h"
#include "radio.h"

#include "eprom.h"

void setupInputs() {

    pinMode(LED, OUTPUT);
    pinMode(WIND, INPUT_PULLUP);
    pinMode(RAIN, INPUT_PULLUP);


}

void setup() {
    //Disable the watchdog
    wdt_disable();


    setupInputs();
    digitalWrite(LED, HIGH);


    DEBUG_BEGIN(115200);
#ifdef DEBUG
    delay(3000);
#endif


    //eeprom::setEEPROM();
    eeprom::loadEEPROM();


    sensors::begin();

    radio::begin();


    digitalWrite(LED, LOW);
}

int tick;

void loop() {
    sensors::update();
    radio::update();
    tick++;

    if(tick == 300) {
        DEBUG_PRINT("lostPackets: ");
        DEBUG_PRINTLN(radio::lostPackets);
        tick = 0;
    }

    delay(1);
}