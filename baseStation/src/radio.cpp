#include "radio.h"
#include "main.h"
#include "eprom.h"

#include <iostream>
#include <queue>
#include <algorithm>
#include <RF24/RF24.h>

using std::cout;
using std::endl;

struct Command {
    //pointer to command array
    uint8_t* command;
    //size of command array
    uint8_t size;
    //expects a reply for the command
    bool expectReply;
};

std::queue<Command> commands;

const uint8_t STATION_ADDRESS[6] = "WTRst";
const uint8_t BASE_ADDRESS[6] = "WTRbs";

#define CE_PIN 25
#define CS_PIN 0

RF24 rad(CE_PIN, CS_PIN);

bool active;

uint16_t radio::rawWindSpeed;
uint16_t radio::rawWindDirection;
uint16_t radio::rawMaxWindSpeed;
uint16_t radio::rawMaxWindDirection;
uint16_t radio::rawAverageWindSpeed;
uint16_t radio::rawAverageWindDirection;

uint16_t radio::rawHumidity;
int16_t radio::rawTemperature;
uint8_t radio::rawRainFall;
uint16_t radio::rawPressure;

uint16_t radio::status::lostPackets;
uint16_t radio::status::chargingTime;
bool radio::status::isCharging;
float radio::status::batteryVoltage;
uint8_t radio::status::numResets;

void radio::begin() {
    global::light(true);
    cout << date() << "> Begin Radio" << endl;

    rad.begin();

    rad.setDataRate(RF24_250KBPS);
    rad.setRetries(15, 15);
    rad.setPayloadSize(PACKET_SIZE);
    
    rad.openWritingPipe(STATION_ADDRESS);
    rad.openReadingPipe(1, BASE_ADDRESS);

    rad.powerDown();
    active = false;

    rad.printDetails();

    cout << date() << "< Begin Radio Complete" << endl;
    global::light(false);
}

bool checkForPacket() {
    cout << date() << "Checking for Packet" << endl;
    if(active) {
        if(rad.available()) {
            cout << date() << "Packet received" << endl;
            uint8_t data[PACKET_SIZE];

            rad.read(data, PACKET_SIZE);

            radio::rawWindSpeed = global::get16(data + WIND_SPEED_LOC);
            radio::rawWindDirection = global::get16(data + WIND_DIREC_LOC);
            radio::rawAverageWindSpeed = global::get16(data + WIND_AVG_SPEED_LOC);
            radio::rawAverageWindDirection = global::get16(data + WIND_AVG_DIREC_LOC);
            radio::rawMaxWindSpeed = global::get16(data + WIND_MAX_SPEED_LOC);
            radio::rawMaxWindDirection = global::get16(data + WIND_MAX_DIREC_LOC);

            radio::rawHumidity = global::get16(data + HUMIDITY_LOC);
            radio::rawTemperature = global::get16(data + TEMPERATURE_LOC);
            radio::rawRainFall = global::get8(data + RAIN_LOC);
            radio::rawPressure = global::get16(data + PRESSURE_LOC);

            return true;
        }
    }

    return false;
}

/**
 * Send the commands stored in commands
 * 
 * @returns true if a response is expected
 */
bool sendPackets() {
    uint8_t packets[PACKET_SIZE];
    //send all of the commands in the queue
    bool expectResponse = false;
    while(!commands.empty()) {
        uint8_t i = 0;
        //stack the commands in the packet as much as it can hold
        Command com = commands.front();
        while(!commands.empty() && i + com.size < PACKET_SIZE) {
            
            cout << date() << "Sending command: " << *com.command << endl;

            com = commands.front();
            uint8_t ind = com.size;
            while(ind--) {
                packets[i + ind] = com.command[ind];
            }
            if(com.expectReply) {
                expectResponse = true;
            }
            i += com.size;
            delete[] com.command;
            commands.pop();
        }
        packets[i] = EOT;
        //if the packet fails, don't send anymore
        if(!rad.write(packets, PACKET_SIZE)) {
            return expectResponse;
        }
    }

    return expectResponse;
}

uint8_t getValue(uint8_t* data);
uint8_t setStatus(uint8_t* data);

void sendCommands() {
    rad.stopListening();
    if(sendPackets()) {
        cout << date() << "Expecting Reply" << endl;
        rad.startListening();
        sleep_for(500ms);
        if(!rad.available()) {
            cout << date() << "Reply didn't come" << endl;
        }
        while(rad.available()) {
            uint8_t data[PACKET_SIZE];

            rad.read(data, PACKET_SIZE);

            uint8_t i = 0;
            while(data[i] != EOT && i < PACKET_SIZE) {
                if(data[i] == COMMAND_GET_VALUE) {
                    i += getValue(data + i + 1);
                    i++;
                } else if(data[i] == COMMAND_GET_STATUS) {
                    i += setStatus(data + i + 1);
                    i++;
                }
                i++;
            }
        }
    }
}

bool radio::update() {
    static unsigned int lost_packets(100);
    bool successfull = false;

    global::light(true);

    cout << date() << "> Waiting for transmission" << endl;

    active = true;
    rad.powerUp();
    rad.startListening();

    //check if the timer is out of sync with the station
    if(lost_packets > 1) {
        cout << date() << "Too many lost packets, possible sync error.  Resyncing" << endl;

        lost_packets = 0;

        //wait for up to eeprom::refreshTime (30s) for an incomming packet
        time_point t =  Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < eeprom::refreshTime) {
            sleep_for(500ms);
            global::toggleLight();
            successfull = checkForPacket();
        }
        global::light(true);


        active = false;
        rad.stopListening();
        rad.powerDown();

        //if a packet was received, wait until the next update time.
        if(successfull) {
            cout << date() << "Packet received, waiting " << (eeprom::refreshTime - eeprom::listenTime / 2) / 1000.0 << " seconds to sync with the station" << endl;
            sleep_for(std::chrono::milliseconds(eeprom::refreshTime - eeprom::listenTime / 2));
        }
        return false;
    } else {

        //wait for an available packet
        time_point t = Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < eeprom::listenTime) {
            sleep_for(500ms);
            successfull = checkForPacket();
        }

        //if successful, try to send the commands
        if(successfull) {
            lost_packets = std::min(lost_packets - 1, (unsigned int)(0));
            sendCommands();
        } else {
            lost_packets++;
        }
    }

    cout << date() << "< " << (successfull ? "Transmission Received" : "No Transmission available") << endl;

    rad.stopListening();
    rad.powerDown();
    active = false;

    global::light(false);

    return successfull;
}


#define LOSTPACKETS_LOC 0
#define CHARGING_TIME_LOC 2
#define IS_CHARGING_LOC 4
#define IS_CHARGING_LOCBIN 0
#define BATTERY_LOC 5
#define RESETS_LOC 6

//location of last item plus its size
#define STATUS_SIZE RESETS_LOC + 1

radio::Status_got successStatus = NULL;

uint8_t setStatus(uint8_t* data) {
    radio::status::lostPackets = global::get16(data + LOSTPACKETS_LOC);
    radio::status::chargingTime = 0;//TODO: implement
    radio::status::isCharging = global::getB(data + IS_CHARGING_LOC, IS_CHARGING_LOCBIN);
    radio::status::batteryVoltage = global::get8(data + BATTERY_LOC) / 50.0;
    radio::status::numResets = global::get8(data + RESETS_LOC);
    if(successStatus) {
        (*successStatus)();
    }
    return STATUS_SIZE;
}

void radio::getStatus(radio::Status_got success) {
    Command c;
    c.size = 1;
    c.command = new uint8_t[c.size];
    c.command[0] = COMMAND_GET_STATUS;
    c.expectReply = true;
    commands.push(c);
    successStatus = success;
}

#define WIND_TICK_SIZE 1
#define WIND_READ_TIME_SIZE 2

#define WIND_AVG_UPDATE_TIME_SIZE 2
#define WIND_AVG_STORAGE_TIME_SIZE 4

#define PRES_ALTITUDE_SIZE 2

#define REFRESH_TIME_SIZE 4

#define RESETS_SIZE 1

std::queue<radio::EEPROM_got> getSuccess;

uint8_t getValue(uint8_t* data) {
    #define GET(size) (size == 1 ? global::get8(data + 1) : \
                            (size == 2 ? global::get16(data + 1) : \
                            global::get32(data + 1)))
    #define GET_NOTIFY(type) if(getSuccess.front())(*getSuccess.front())(type); \
                             getSuccess.pop()

    switch(*data) {
    case WIND_TICK:
        eeprom::station::wind::ticks = GET(WIND_TICK_SIZE);
        GET_NOTIFY(WIND_TICK);
        return WIND_TICK_SIZE;
    case WIND_READ_TIME:
        eeprom::station::wind::readTime = GET(WIND_READ_TIME_SIZE);
        GET_NOTIFY(WIND_READ_TIME);
        return WIND_READ_TIME_SIZE;
    case WIND_AVG_UPDATE_TIME:
        eeprom::station::wind::averageUpdateTime = GET(WIND_AVG_UPDATE_TIME_SIZE);
        GET_NOTIFY(WIND_AVG_UPDATE_TIME);
        return WIND_AVG_UPDATE_TIME_SIZE;
    case WIND_AVG_STORAGE_TIME:
        eeprom::station::wind::averageStorageTime = GET(WIND_AVG_STORAGE_TIME_SIZE);
        GET_NOTIFY(WIND_AVG_STORAGE_TIME);
        return WIND_AVG_STORAGE_TIME_SIZE;
    case PRES_ALTITUDE:
        eeprom::station::pressure::altitude = GET(PRES_ALTITUDE_SIZE);
        GET_NOTIFY(PRES_ALTITUDE);
        return PRES_ALTITUDE_SIZE;
    case REFRESH_TIME:
        eeprom::refreshTime = GET(REFRESH_TIME_SIZE);
        GET_NOTIFY(REFRESH_TIME);
        return REFRESH_TIME_SIZE;
    }
    return 0;
    
}

void radio::getEEPROM(EEPROM_Variable variable, radio::EEPROM_got success) {
    Command c;
    c.size = 2;
    c.command = new uint8_t[c.size];
    c.expectReply = true;

    c.command[0] = COMMAND_GET_VALUE;
    c.command[1] = variable;
    commands.push(c);
    getSuccess.push(success);
}


void initCommand(uint8_t size, Command &c) {
    c.size = size + 2;
    c.command = new uint8_t[c.size];
    commands.push(c);
}

template<typename T>
void radio::setEEPROM(EEPROM_Variable var, T value) {
//populate the command array
#define SET(size) c.command[0] = COMMAND_SET_VALUE; \
                  c.command[1] = var; \
                  (size == 1 ? global::set8(c.command + 2, value) : \
                  (size == 2 ? global::set16(c.command + 2, value) : \
                  global::set32(c.command + 2, value)))
    Command c;
    c.expectReply = false;

    switch(var) {
    case WIND_TICK:
        initCommand(WIND_TICK_SIZE, c);
        SET(WIND_TICK_SIZE);
        return;
    case WIND_READ_TIME:
        initCommand(WIND_READ_TIME_SIZE, c);
        SET(WIND_READ_TIME_SIZE);
        return;
    case WIND_AVG_UPDATE_TIME:
        initCommand(WIND_AVG_UPDATE_TIME_SIZE, c);
        SET(WIND_AVG_UPDATE_TIME_SIZE);
        return;
    case WIND_AVG_STORAGE_TIME:
        initCommand(WIND_AVG_STORAGE_TIME_SIZE, c);
        SET(WIND_AVG_STORAGE_TIME_SIZE);
        return;
    case PRES_ALTITUDE:
        initCommand(PRES_ALTITUDE_SIZE, c);
        SET(PRES_ALTITUDE_SIZE);
        return;
    case REFRESH_TIME:
        initCommand(REFRESH_TIME_SIZE, c);
        SET(REFRESH_TIME_SIZE);
        return;
    }
}