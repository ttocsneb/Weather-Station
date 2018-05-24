#include "sensors.h"
#include "radio.h"
#include "main.h"

#include "eprom.h"

#include <avr/wdt.h>

void setupInputs() {

    pinMode(LED, OUTPUT);
    pinMode(WIND, INPUT_PULLUP);
    pinMode(RAIN, INPUT_PULLUP);

    pinMode(BATTPIN, INPUT);

    pinMode(SOLARVOLTAGEPIN, INPUT);
    pinMode(SOLARENABLEPIN, OUTPUT);
    digitalWrite(SOLARENABLEPIN, HIGH);


}

bool resetChip = false;

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
    eeprom::resets += 1;
    eeprom::setEEPROM();


    sensors::begin();

    radio::begin();


    digitalWrite(LED, LOW);

    //enable the watchdog to reset after 2 seconds of inactivity
    wdt_enable(WDTO_2S);
}

bool main::isCharging = false;
uint16_t main::chargeTime = 0;
uint32_t main::uptime = 0;

void loop() {

    //Calculate the uptime in seconds with a limit of 136 years before the int overflow
    static unsigned long lastLoopTime(0);
    static unsigned int uptimeSeconds(0);
    uptimeSeconds += (millis() - lastLoopTime);
    if(uptimeSeconds > 1000) {
        if(lastLoopTime <= millis()) {
            main::uptime += uptimeSeconds / 1000;

            DEBUG_PRINT("Uptime: ");
            DEBUG_PRINTLN(main::uptime);

            //add to chargeTime if charging
            if(main::isCharging) {
                main::chargeTime += uptimeSeconds / 1000;
            }
            
            uptimeSeconds = uptimeSeconds % 1000;
        } else {
            uptimeSeconds = 0;
        }

        //Reset the watchdog once every second
        if(!resetChip) {
            DEBUG_PRINTLN("Reset Watchdog timer");
            wdt_reset();
        }

        DEBUG_PRINT("Batt Voltage: ");
        DEBUG_PRINTLN(main::getSolarVoltage());

    }
    lastLoopTime = millis();

    sensors::update();
    radio::update();

    main::updateBattery();
    

    delay(1);
}

void main::updateBattery() {
    static unsigned long lastChargeTime(0);
    static unsigned long lastBatteryPoll(0);
    if(millis() - lastBatteryPoll > BATTERY_POLL_TIME) {
        lastBatteryPoll = millis();
        float charge = getSolarVoltage();
        int batt = round(getBatteryVoltage() * 100);
        if(!isCharging && charge > 4.8 && batt < 420) {
            digitalWrite(SOLARENABLEPIN, LOW);
            long t = millis();
            //run main loop while waiting
            while(t - millis() < 500) {
                loop();
            }
            if(getSolarVoltage() < 4) {
                digitalWrite(SOLARENABLEPIN, HIGH);
            } else {
                isCharging = true;
                //Reset the charge time if the time after stopping charge 
                //is greater than the threshold
                if(millis() - lastChargeTime > BATTERY_CHARGE_TIME_RESET_THRESHOLD) {
                    chargeTime = 0;
                }
            }
        } else if(isCharging && (charge < 4 || batt == 420)) {
            isCharging = false;
            lastChargeTime = millis();
            digitalWrite(SOLARENABLEPIN, HIGH);
        }

        DEBUG_PRINT("CHARGE STATUS: ");
        DEBUG_PRINTLN(isCharging ? "Charging" : "Not Charging");
    }
}

void main::reset() {
    resetChip = true;
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

#define LOSTPACKETS_LOC 0
#define CHARGING_TIME_LOC 2
#define IS_CHARGING_LOC 4
#define IS_CHARGING_LOCBIN 0
#define BATTERY_LOC 5
#define RESETS_LOC 6
#define UPTIME_LOC 7

//location of last item plus its size
#define STATUS_SIZE UPTIME_LOC + 4

void main::loadStatus(uint8_t* data) {
    set16(data + LOSTPACKETS_LOC, radio::lostPackets);
    set16(data + CHARGING_TIME_LOC, chargeTime);
    setB(data + IS_CHARGING_LOC, IS_CHARGING_LOCBIN, isCharging);
    set8(data + BATTERY_LOC, static_cast<uint8_t>(getBatteryVoltage()*50));
    set8(data + RESETS_LOC, eeprom::resets);
    set32(data + UPTIME_LOC, uptime);
}

uint8_t main::getStatusSize() {
    return STATUS_SIZE;
}