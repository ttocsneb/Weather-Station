#include "eprom.h"

#include <EEPROM.h>
#define DEBUG
#include <debug.h>

uint8_t eeprom::resets = 0;
uint32_t eeprom::refreshTime = 30000;

uint8_t eeprom::wind::ticks = 8;
uint16_t eeprom::wind::readTime = 2000;
uint16_t eeprom::wind::tickDelay = 250;
uint16_t eeprom::wind::tickMultiplier = 15;

uint16_t eeprom::wind::averageUpdateTime = 1000;
uint32_t eeprom::wind::averageStorageTime = 120000;
uint8_t eeprom::wind::averageTimeN = 120;

uint16_t eeprom::pressure::altitude = 1409;



uint8_t load8(uint16_t loc) {
    return EEPROM.read(loc);
}

void set8(uint16_t loc, uint8_t val) {
    EEPROM.update(loc, val);
}

uint16_t load16(uint16_t loc) {
    return (static_cast<uint16_t>(load8(loc)) << 8) | load8(loc + 1);
}

void set16(uint16_t loc, uint16_t val) {
    set8(loc, (val >> 8) & 0xFF);
    set8(loc + 1, val & 0xFF);
}

uint32_t load32(uint16_t loc) {
    return (static_cast<uint32_t>(load16(loc)) << 16) | load16(loc + 2);
}

void set32(uint16_t loc, uint32_t val) {
    set16(loc, (val >> 16) & 0xFFFF);
    set16(loc + 2, val & 0xFFFF);
}


void eeprom::loadEEPROM() {
    wind::ticks = load8(WIND_TICK);
    wind::readTime = load16(WIND_READ_TIME);
    wind::tickDelay = wind::readTime / wind::ticks;
    wind::tickMultiplier = 60000 / wind::readTime / 2;

    wind::averageStorageTime = load32(WIND_AVG_STORAGE_TIME);
    wind::averageUpdateTime = load16(WIND_AVG_UPDATE_TIME);
    wind::averageTimeN = wind::averageStorageTime / wind::averageUpdateTime;

    DEBUG_PRINTLN(wind::averageStorageTime);
    DEBUG_PRINTLN(wind::averageUpdateTime);
    DEBUG_PRINTLN(wind::averageTimeN);

    pressure::altitude = load16(PRES_ALTITUDE);

    resets = load8(RESETS);
    refreshTime = load32(REFRESH_TIME);
}

void eeprom::setEEPROM() {
    set8(WIND_TICK, wind::ticks);
    set16(WIND_READ_TIME, wind::readTime);
    wind::tickDelay = wind::readTime / wind::ticks;
    wind::tickMultiplier = 60000 / wind::readTime / 2;

    set32(WIND_AVG_STORAGE_TIME, wind::averageStorageTime);
    set16(WIND_AVG_UPDATE_TIME, wind::averageUpdateTime);
    wind::averageTimeN = wind::averageStorageTime / wind::averageUpdateTime;

    set16(PRES_ALTITUDE, pressure::altitude);

    set8(RESETS, resets);
    set32(REFRESH_TIME, refreshTime);
}