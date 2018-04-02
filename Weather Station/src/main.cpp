#include "main.h"
#include "sensors.h"

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
    DEBUG_PRINTLN("OPENING DEBUGGER");
    delay(3000);
    DEBUG_PRINTLN("OPENING DEBUGGER");
#endif


    //eeprom::setEEPROM();
    eeprom::loadEEPROM();


    sensors::begin();




    digitalWrite(LED, LOW);
}

int tick;

void loop() {
    sensors::update();
    tick++;

    if(tick == 300) {
        DEBUG_PRINT("wind: ");
        DEBUG_PRINTLN(sensors::getWindSpeed());
        DEBUG_PRINT("max: ");
        DEBUG_PRINTLN(sensors::getMaxWindSpeed());
        DEBUG_PRINT("avg: ");
        DEBUG_PRINTLN(sensors::getAverageWindSpeed());
        DEBUG_PRINT("direction: ");
        DEBUG_PRINTLN(sensors::getWindDirection());
        DEBUG_PRINT("max: ");
        DEBUG_PRINTLN(sensors::getMaxWindDirection());
        DEBUG_PRINT("avg: ");
        DEBUG_PRINTLN(sensors::getAverageWindDirection());
        tick = 0;
    }

    delay(1);
}