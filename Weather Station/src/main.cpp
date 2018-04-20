#include "sensors.h"
#include "radio.h"
#include "main.h"

#include "eprom.h"

void setupInputs() {

    pinMode(LED, OUTPUT);
    pinMode(WIND, INPUT_PULLUP);
    pinMode(RAIN, INPUT_PULLUP);

    pinMode(BATTPIN, INPUT);

    pinMode(SOLARVOLTAGEPIN, INPUT);
    pinMode(SOLARENABLEPIN, OUTPUT);


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

bool main::isCharging = false;

void loop() {
    static long lastBatteryPoll(0);

    sensors::update();
    radio::update();
    tick++;

    if(tick == 300) {
        DEBUG_PRINT("lostPackets: ");
        DEBUG_PRINTLN(radio::lostPackets);
        tick = 0;

        DEBUG_PRINT("SOLAR VOLTAGE:");
        DEBUG_PRINTLN(main::getSolarVoltage(), 5);
    }

    if(millis() - lastBatteryPoll > BATTERY_POLL_TIME) {
        lastBatteryPoll = millis();
        float charge = main::getSolarVoltage();
        if(!main::isCharging && charge > 5.0) {
            digitalWrite(SOLARENABLEPIN, HIGH);
            long t = millis();
            while(t - millis() < 500) {
                loop();
            }
            if(main::getSolarVoltage() < 2.5) {
                digitalWrite(SOLARENABLEPIN, LOW);
            } else {
                charge = true;
            }
        } else if(main::isCharging && charge < 2.5) {
            charge = false;
            digitalWrite(SOLARENABLEPIN, LOW);
        }

        DEBUG_PRINT("CHARGE STATUS: ");
        DEBUG_PRINTLN(main::isCharging ? "Charging" : "Not Charging");
    }

    delay(1);
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
    return (analogRead(SOLARVOLTAGEPIN) / 1024.0) * 3.3 * 3.1304;
}

float main::getBatteryVoltage() {
    return (analogRead(BATTPIN) / 512.0) * 3.3;
}

#define LOSTPACKETS_LOC 0
#define CHARGING_TIME_LOC 2
#define IS_CHARGING_LOC 4
#define IS_CHARGING_LOCBIN 0
#define BATTERY_LOC 5
#define RESETS_LOC 6

//location of last item plus its size
#define STATUS_SIZE RESETS_LOC + 1

void main::loadStatus(uint8_t* data) {
    set16(data + LOSTPACKETS_LOC, radio::lostPackets);
    //TODO: set CHARGING_TIME
    setB(data + IS_CHARGING_LOC, IS_CHARGING_LOCBIN, isCharging);
    set8(data + BATTERY_LOC, static_cast<uint8_t>(getBatteryVoltage()*50));
    set8(data + RESETS_LOC, eeprom::resets);
}

uint8_t main::getStatusSize() {
    return STATUS_SIZE;
}