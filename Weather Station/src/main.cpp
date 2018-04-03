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


    eeprom::setEEPROM();
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

void main::set8(uint8_t* index, uint8_t val) {
    *index = val;
}

void main::set16(uint8_t* index, uint16_t val) {
    set8(index, (val >> 8) & 0xFF);
    set8(index + 1, val & 0xFF);
}

void main::set32(uint8_t* index, uint32_t val) {
    set16(index, (val >> 16) & 0xFFFF);
    set16(index + 2, val & 0xFFFF);
}

uint8_t main::get8(uint8_t* index) {
    return *index;
}

uint16_t main::get16(uint8_t* index) {
    return (static_cast<uint16_t>(get8(index)) << 8) | get8(index + 1);
}

uint32_t main::get32(uint8_t* index) {
    return (static_cast<uint32_t>(get16(index)) << 16) | get16(index + 2);
}