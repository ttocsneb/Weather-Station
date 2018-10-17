#include "eprom.h"

#include "main.h"

#include <EEPROM.h>
#define DEBUG
#include <debug.h>

#define DEFAULT_REFRESH_TIME 30000
#define DEFAULT_LISTEN_TIME 500

#define DEFAULT_WIND_STORAGE 2000
#define DEFAULT_WIND_UPDATE 250

#define DEFAULT_AVG_WIND_UPDATE 1000
#define DEFAULT_AVG_WIND_STORAGE 120000

#define DEFAULT_ALTITUDE 0

uint32_t eeprom::refresh_time = DEFAULT_REFRESH_TIME;

uint16_t eeprom::wind::storage = DEFAULT_WIND_STORAGE;
uint16_t eeprom::wind::update = DEFAULT_WIND_UPDATE;
uint8_t eeprom::wind::queue_size = DEFAULT_WIND_STORAGE / DEFAULT_WIND_UPDATE;

uint16_t eeprom::wind::average_update = DEFAULT_AVG_WIND_UPDATE;
uint32_t eeprom::wind::average_storage = DEFAULT_AVG_WIND_STORAGE;
uint8_t eeprom::wind::average_queue_size = DEFAULT_AVG_WIND_STORAGE / DEFAULT_AVG_WIND_UPDATE;

uint16_t eeprom::pressure::altitude = DEFAULT_ALTITUDE;

uint16_t eeprom::listen_time = DEFAULT_LISTEN_TIME;

uint16_t eeprom::shortest_delay = eeprom::wind::update;



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

void update_constants() {
    eeprom::wind::queue_size = eeprom::wind::storage / eeprom::wind::update;
    eeprom::wind::average_queue_size = eeprom::wind::average_storage / eeprom::wind::average_update;


    // Calculate shortest delay
    unsigned int delays[] = {
        eeprom::refresh_time,
        eeprom::listen_time,
        eeprom::wind::average_update,
        eeprom::wind::update
    };
    eeprom::shortest_delay = 0x7FFF;

    for(int delay : delays) {
        eeprom::shortest_delay = min(eeprom::shortest_delay, delay);
    }
}

void eeprom::loadEEPROM() {
    wind::update = load8(WIND_UPDATE);
    wind::storage = load16(WIND_STORAGE);

    wind::average_storage = load32(AVG_WIND_STORAGE);
    wind::average_update = load16(AVG_WIND_UPDATE);

    DEBUG_PRINTLN(wind::average_storage);
    DEBUG_PRINTLN(wind::average_update);
    DEBUG_PRINTLN(wind::average_queue_size);

    pressure::altitude = load16(ALTITUDE);

    refresh_time = load32(REFRESH_TIME);
    listen_time = load16(LISTEN_TIME);

    update_constants();
}

void eeprom::setEEPROM() {
    set8(WIND_UPDATE, wind::update);
    set16(WIND_STORAGE, wind::storage);

    set32(AVG_WIND_STORAGE, wind::average_storage);
    set16(AVG_WIND_UPDATE, wind::average_update);

    set16(ALTITUDE, pressure::altitude);

    set32(REFRESH_TIME, refresh_time);
    set32(LISTEN_TIME, listen_time);

    update_constants();
}

void eeprom::loadPacket(uint8_t* packet) {
    main::set32(packet + 0, refresh_time);
    main::set16(packet + 4, listen_time);
    main::set16(packet + 6, pressure::altitude);
    main::set16(packet + 8, wind::average_update);
    main::set32(packet + 10, wind::average_storage);
    main::set16(packet + 14, wind::update);
    main::set16(packet + 16, wind::storage);
}

void eeprom::reset() {
    wind::update = DEFAULT_WIND_UPDATE;
    wind::storage = DEFAULT_WIND_STORAGE;

    wind::average_update = DEFAULT_AVG_WIND_UPDATE;
    wind::average_storage = DEFAULT_AVG_WIND_STORAGE;

    pressure::altitude = DEFAULT_ALTITUDE;

    refresh_time = DEFAULT_REFRESH_TIME;
    listen_time = DEFAULT_LISTEN_TIME;

    update_constants();
}