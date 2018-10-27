#include "sensors.h"
#include "radio.h"
#include "main.h"

#include "eprom.h"

#include <avr/wdt.h>


bool resetChip = false;
bool main::isCharging = false;
bool main::justStarted = true;

void setupInputs() {

    pinMode(LED, OUTPUT);
    pinMode(WIND, INPUT_PULLUP);
    pinMode(RAIN, INPUT_PULLUP);

    pinMode(BATTPIN, INPUT);

    pinMode(SOLARVOLTAGEPIN, INPUT);
    pinMode(SOLARENABLEPIN, OUTPUT);
    digitalWrite(SOLARENABLEPIN, HIGH);


}


void setup() {
    //Disable the watchdog
    resetChip = false;
    wdt_disable();


    setupInputs();
    digitalWrite(LED, HIGH);


    DEBUG_BEGIN(115200);
#ifdef DEBUG
    delay(3000);
#else
    delay(500);
#endif

    eeprom::loadEEPROM();

    sensors::begin();

    radio::begin();

    digitalWrite(LED, LOW);

    //enable the watchdog to reset after 2 seconds of inactivity
    wdt_enable(WDTO_2S);
}


void loop() {
    radio::update();

    main::updateBattery();
    sensors::update();

    if(!resetChip) {
        wdt_reset();
    }
}


void main::updateBattery() {
    static unsigned long lastBatteryPoll(0);

    // Only poll the battery after BATTERY_POLL_TIME ms
    if(millis() - lastBatteryPoll > BATTERY_POLL_TIME) {
        lastBatteryPoll = millis();
        float charge = getSolarVoltage();
        int batt = round(getBatteryVoltage() * 100);
        if(!isCharging && charge > 4.8 && batt < 420) {
            digitalWrite(SOLARENABLEPIN, LOW);
            isCharging = true;

        } else if(isCharging && (charge < 4 || batt == 420)) {
            digitalWrite(SOLARENABLEPIN, HIGH);
            isCharging = false;
        }

        DEBUG_PRINT("CHARGE STATUS: ");
        DEBUG_PRINTLN(isCharging ? "Charging" : "Not Charging");
    }
}


void main::restart() {
    resetChip = true;
}


void main::reset() {
    radio::lostPackets = 0;
    justStarted = false;
}


void main::setB(uint8_t* index, uint8_t location, bool val) {
    if(val) {
        *index = *index | (1 << location);
    } else {
        *index = *index & ~(1 << location);
    }
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


bool main::getB(uint8_t* index, uint8_t location) {
    return (*index >> location) & 1;
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


float main::getSolarVoltage() {
    return (analogRead(SOLARVOLTAGEPIN) / 1024.0) * 3.3 * 2;
}


float main::getBatteryVoltage() {
    return (analogRead(BATTPIN) / 512.0) * 3.3;
}

float main::getBatteryTemperature() {
    // TODO: Implement!
    return 0;
}


void main::loadStatus(uint8_t* data) {

    setB(data, 0, justStarted);
    setB(data, 1, isCharging);

    set8(data + 1, static_cast<uint8_t>(getBatteryVoltage() * 100 - 165));

    set16(data + 2, static_cast<int16_t>(getBatteryTemperature() * 10));

    set8(data + 4, radio::lostPackets);
}