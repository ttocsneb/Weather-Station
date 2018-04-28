#include "commands.h"
#include "main.h"
#include "eprom.h"
#include "radio.h"

#include <iostream>
#include <queue>

using std::cout;
using std::endl;

uint8_t commands::station::wind::ticks;
uint16_t commands::station::wind::readTime;

uint16_t commands::station::wind::averageUpdateTime;
uint32_t commands::station::wind::averageStorageTime;

uint16_t commands::station::pressure::altitude;

uint32_t commands::station::refreshTime;

uint16_t commands::status::lostPackets;
uint16_t commands::status::chargingTime;
bool commands::status::isCharging;
float commands::status::batteryVoltage;
uint8_t commands::status::numResets;

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
std::queue<commands::EEPROM_Got> getSuccess;
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

void commands::getEEPROM(EEPROM_Variable variable, commands::EEPROM_Got success) {
    Command c = addCommand(2, true);
    c.command[0] = COMMAND_GET_VALUE;
    c.command[1] = variable;
    getSuccess.push(success);
}

void commands::getStatus(commands::Status_got success) {
    Command c = addCommand(1, true);
    *c.command = COMMAND_GET_STATUS;
    statusSuccess = success;
}

void commands::saveEEPROM() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_SET_EEPROM;
}

void commands::loadEEPROM() {
    Command c = addCommand(1, false);
    *c.command = COMMAND_LOAD_EEPROM;
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
 * Get value from commands::getEEPROM()
 * 
 * @param data location of data
 * 
 * @return number of bytes in command
 */
uint8_t getValue(const uint8_t* data) {
#define GET(size) (size == 1 ? global::get<uint8_t>(data + 1) : \
        (size == 2 ? global::get<uint16_t>(data + 1) : \
        global::get<uint32_t>(data + 1)))
#define GET_NOTIFY(type) if(getSuccess.front())(*getSuccess.front())(type); \
        getSuccess.pop()
    
    switch(*data) {
    case ALIAS_WIND_TICK:
        commands::station::wind::ticks = GET(WIND_TICK_SIZE);
        GET_NOTIFY(ALIAS_WIND_TICK);
        return WIND_TICK_SIZE;
    case ALIAS_WIND_READ_TIME:
        commands::station::wind::readTime = GET(WIND_READ_TIME_SIZE);
        GET_NOTIFY(ALIAS_WIND_READ_TIME);
        return WIND_READ_TIME_SIZE;
    case ALIAS_WIND_AVG_UPDATE_TIME:
        commands::station::wind::averageUpdateTime = GET(WIND_AVG_UPDATE_TIME_SIZE);
        GET_NOTIFY(ALIAS_WIND_AVG_UPDATE_TIME);
        return WIND_AVG_UPDATE_TIME_SIZE;
    case ALIAS_WIND_AVG_STORAGE_TIME:
        commands::station::wind::averageStorageTime = GET(WIND_AVG_STORAGE_TIME_SIZE);
        GET_NOTIFY(ALIAS_WIND_AVG_STORAGE_TIME);
        return WIND_AVG_STORAGE_TIME_SIZE;
    case ALIAS_PRES_ALTITUDE:
        commands::station::pressure::altitude = GET(PRES_ALTITUDE_SIZE);
        GET_NOTIFY(ALIAS_PRES_ALTITUDE);
        return PRES_ALTITUDE_SIZE;
    case ALIAS_REFRESH_TIME:
        commands::station::refreshTime = GET(REFRESH_TIME_SIZE);
        GET_NOTIFY(ALIAS_REFRESH_TIME);
        return REFRESH_TIME_SIZE;
    }
    return 0;

#undef GET
#undef GET_NOTIFY
}

/**
 * Get status from commands::getStatus()
 * 
 * @param data location of data
 * 
 * @return number of bytes in command
 */
uint8_t readStatus(const uint8_t* data) {
    const uint8_t LOSTPACKETS_LOC = 0;
    const uint8_t CHARGING_TIME_LOC = 2;
    const uint8_t IS_CHARGING_LOC = 4;
    const uint8_t IS_CHARGING_LOCBIN = 0;
    const uint8_t BATTERY_LOC = 5;
    const uint8_t RESETS_LOC = 6;

    //location of last item plus its size
    const uint8_t STATUS_SIZE = RESETS_LOC + 1;

    commands::status::lostPackets = global::get<uint16_t>(data + LOSTPACKETS_LOC);
    commands::status::chargingTime = 0;//TODO: implement
    commands::status::isCharging = global::getBool(data + IS_CHARGING_LOC, IS_CHARGING_LOCBIN);
    commands::status::batteryVoltage = global::get<uint8_t>(data + BATTERY_LOC) / 50.0;
    commands::status::numResets = global::get<uint8_t>(data + RESETS_LOC);

    if(statusSuccess) {
        (*statusSuccess)();
    }

    return STATUS_SIZE;

}

bool commands::loadCommands(uint8_t* packet, uint8_t size) {
    uint8_t i = 0;
    bool expectResponse = false;
    Command com = qCommands.front();

    cout << date() << "Sending commands to Station:" << endl;

    while(!qCommands.empty() && i + com.size < size) {

        cout << "Sending Command: " << *com.command << endl;

        uint8_t ind = com.size;
        while(ind--) {
            packet[i + ind] = com.command[ind];
        }
        if(com.expectReply) {
            expectResponse = true;
        }
        i += com.size;
        deleteCommand(com);
        qCommands.pop();
        com = qCommands.front();
    }

    packet[i] = EOT;

    return expectResponse;
}

void commands::getReply(const uint8_t* packet, uint8_t size) {
    uint8_t i = 0;
    while(packet[i] != EOT && i < size) {
        if(packet[i] == COMMAND_GET_VALUE) {
            i += getValue(packet + i + 1);
            i++;
        } else if(packet[i] == COMMAND_GET_STATUS) {
            i += readStatus(packet + i + 1);
            i++;
        }
        i++;
    }
}