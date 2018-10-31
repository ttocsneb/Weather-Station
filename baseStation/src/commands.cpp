#include "commands.h"
#include "main.h"
#include "eprom.h"
#include "radio.h"

#include <iostream>
#include <queue>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

TYPE_WIND_UPDATE commands::eeprom::wind::wind_update = 250;
TYPE_WIND_STORAGE commands::eeprom::wind::wind_storage = 2000;

TYPE_AVG_WIND_UPDATE commands::eeprom::wind::avg_wind_update = 1000;
TYPE_AVG_WIND_STORAGE commands::eeprom::wind::avg_wind_storage = 120000;

TYPE_ALTITUDE commands::eeprom::pressure::altitude = 0;

TYPE_REFRESH_TIME commands::eeprom::refresh_time = 30000;
TYPE_LISTEN_TIME commands::eeprom::listen_time = 500;


bool commands::status::just_started;
bool commands::status::charging;
float commands::status::battery;
float commands::status::battery_temp;
uint8_t commands::status::lost_packets;


uint16_t commands::weather::rawWindSpeed;
uint8_t  commands::weather::rawWindDirection;
uint16_t commands::weather::rawMaxWindSpeed;
uint8_t  commands::weather::rawMaxWindDirection;
uint16_t commands::weather::rawAverageWindSpeed;
uint16_t commands::weather::rawAverageWindDirection;

uint8_t commands::weather::rawHumidity;
int16_t commands::weather::rawTemperature;
uint8_t commands::weather::rawRainFall;
uint16_t commands::weather::rawPressure;

//==================== Command Queue ====================

struct Command {
    //pointer to command array
    uint8_t* command;
    //size of command array
    uint8_t size;
    //expects a reply for the command
    bool expectReply;
};

std::queue<Command> qCommands;
commands::EEPROM_Got eepromSuccess;
commands::Status_got statusSuccess;

Command addCommand(uint8_t size, bool expectReply) {
    Command c;
    c.expectReply = expectReply;
    c.size = size;
    c.command = new uint8_t[size];
    qCommands.push(c);
    return c;
}

void deleteCommand(Command c) {
    if(c.command) {
        delete[] c.command;
        c.command = NULL;
    }
    return;
}

//==================== Command functions ====================

template<typename T>
void commands::setEEPROM(EEPROM_Variable variable, T value) {
    Command c = addCommand(2 + sizeof(T), false);

    c.command[0] = COMMAND_SET_VALUE;
    c.command[1] = variable;
    global::set<T>(c.command + 2, value);
}
template void commands::setEEPROM<uint8_t>(EEPROM_Variable variable, uint8_t value);
template void commands::setEEPROM<uint16_t>(EEPROM_Variable variable, uint16_t value);
template void commands::setEEPROM<uint32_t>(EEPROM_Variable variable, uint32_t value);

void commands::getEEPROM(commands::EEPROM_Got success) {
    Command c = addCommand(1, true);
    c.command[0] = COMMAND_GET_SETTINGS;
    eepromSuccess = success;
}

void commands::getStatus(commands::Status_got success) {
    Command c = addCommand(1, true);
    *c.command = COMMAND_GET_STATUS;
    statusSuccess = success;
}

void commands::resetWeather() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_RESET;
}

void commands::resetStatus() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_RESET_STATUS;
}

void commands::restart() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_RESTART;
}

void commands::resetEEPROM() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_RESET_SETTINGS;
}


//==================== Process COmmands ====================

#define WIND_TICK_SIZE 1
#define WIND_READ_TIME_SIZE 2

#define WIND_AVG_UPDATE_TIME_SIZE 2
#define WIND_AVG_STORAGE_TIME_SIZE 4

#define PRES_ALTITUDE_SIZE 2

#define REFRESH_TIME_SIZE 4

#define RESETS_SIZE 1

/**
 * Get status from commands::getStatus()
 * 
 * @param data location of data
 * 
 */
void readStatus(const uint8_t* data) {
    const uint8_t BOOLEANS = 0;

    const uint8_t JUST_STARTED_BIN = 0;
    const uint8_t CHARGING_BIN = 1;

    const uint8_t BATTERY = 1;
    const uint8_t BATT_TEMP = 2;
    const uint8_t LOST_PACKETS = 4;

    commands::status::just_started = global::getBool(data + BOOLEANS, JUST_STARTED_BIN);
    commands::status::charging = global::getBool(data + BOOLEANS, CHARGING_BIN);

    // load and convert the battery voltage/temperature
    uint8_t batt = global::get<uint8_t>(data + BATTERY);
    commands::status::battery = (batt / 100.0) + 1.65;
    int16_t batt_temp = global::get<int16_t>(data + BATT_TEMP);
    commands::status::battery_temp = batt_temp / 10.0;

    commands::status::lost_packets = global::get<uint8_t>(data + LOST_PACKETS);

    if(statusSuccess) {
        (*statusSuccess)();
    }
}

/**
 * Get EEPROM from commands::getEEPROM()
 */
bool readEEPROM(const uint8_t* data) {
    const uint8_t WEATHER_INTERVAL = 0;
    const uint8_t LISTEN_TIME = 4;
    const uint8_t ALTITUDE = 6;
    const uint8_t AVG_WIND_UPDATE = 8;
    const uint8_t AVG_WIND_STORAGE = 10;
    const uint8_t WIND_UPDATE = 14;
    const uint8_t WIND_STORAGE = 16;

    commands::eeprom::refresh_time = global::get<TYPE_REFRESH_TIME>(data + WEATHER_INTERVAL);
    commands::eeprom::listen_time = global::get<TYPE_LISTEN_TIME>(data + LISTEN_TIME);
    commands::eeprom::pressure::altitude = global::get<TYPE_ALTITUDE>(data + ALTITUDE);
    commands::eeprom::wind::avg_wind_update = global::get<TYPE_AVG_WIND_UPDATE>(data + AVG_WIND_UPDATE);
    commands::eeprom::wind::avg_wind_storage = global::get<TYPE_AVG_WIND_STORAGE>(data + AVG_WIND_STORAGE);
    commands::eeprom::wind::wind_update = global::get<TYPE_WIND_UPDATE>(data + WIND_UPDATE);
    commands::eeprom::wind::wind_storage = global::get<TYPE_WIND_STORAGE>(data + WIND_STORAGE);

    if(eepromSuccess) {
        (*eepromSuccess)();
    }
}

bool commands::loadCommand(uint8_t* packet, uint8_t size) {
    uint8_t i = 0;
    bool expectResponse = false;

    if(!qCommands.empty()) {
        Command com = qCommands.front();
        uint8_t ind = size < com.size ? size : com.size;
        while(ind--) {
            packet[ind] = com.command[ind];
        }
        if(com.expectReply) {
            expectResponse = true;
        }
        deleteCommand(com);
        qCommands.pop();
    }

    return expectResponse;
}


void loadRawWeather(const uint8_t* data) {

    commands::weather::rawWindDirection = global::get<uint8_t>(data + 0);
    commands::weather::rawMaxWindDirection = global::get<uint8_t>(data + 1);
    commands::weather::rawAverageWindDirection = global::get<uint16_t>(data + 2);
    commands::weather::rawWindSpeed = global::get<uint16_t>(data + 4);
    commands::weather::rawMaxWindSpeed = global::get<uint16_t>(data + 6);
    commands::weather::rawAverageWindSpeed = global::get<uint16_t>(data + 8);

    commands::weather::rawRainFall = global::get<uint8_t>(data + 10);
    commands::weather::rawTemperature = global::get<uint16_t>(data + 11);
    commands::weather::rawHumidity = global::get<uint8_t>(data + 13);
    commands::weather::rawPressure = global::get<uint16_t>(data + 14);
}


void commands::getReply(const uint8_t* packet, uint8_t size) {
    uint8_t i = 0;

    uint8_t command = packet[0];

    if(command == COMMAND_GET_WEATHER) {
        loadRawWeather(packet + 1);
    } else if(command == COMMAND_GET_SETTINGS) {
        //TODO: Process get settings Command
    } else if(command == COMMAND_GET_STATUS) {
        readStatus(packet + 1);
    }
}
