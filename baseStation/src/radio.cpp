#include "radio.h"
#include "main.h"
#include "eprom.h"
#include "commands.h"

#include <iostream>
#include <queue>
#include <algorithm>
#include <RF24/RF24.h>

using std::cout;
using std::endl;

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

            radio::rawWindSpeed = global::get<uint16_t>(data + WIND_SPEED_LOC);
            radio::rawWindDirection = global::get<uint16_t>(data + WIND_DIREC_LOC);
            radio::rawAverageWindSpeed = global::get<uint16_t>(data + WIND_AVG_SPEED_LOC);
            radio::rawAverageWindDirection = global::get<uint16_t>(data + WIND_AVG_DIREC_LOC);
            radio::rawMaxWindSpeed = global::get<uint16_t>(data + WIND_MAX_SPEED_LOC);
            radio::rawMaxWindDirection = global::get<uint16_t>(data + WIND_MAX_DIREC_LOC);

            radio::rawHumidity = global::get<uint16_t>(data + HUMIDITY_LOC);
            radio::rawTemperature = global::get<uint16_t>(data + TEMPERATURE_LOC);
            radio::rawRainFall = global::get<uint8_t>(data + RAIN_LOC);
            radio::rawPressure = global::get<uint16_t>(data + PRESSURE_LOC);

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

    bool expectResponse = commands::loadCommands(packets, PACKET_SIZE);

    if(!rad.write(packets, PACKET_SIZE)) {
        cout << date() << "ERROR: Could not send Commands" << endl;
    }

    return expectResponse;
}

void sendCommands() {
    rad.stopListening();
    if(sendPackets()) {
        cout << date() << "Expecting Reply" << endl;
        rad.startListening();
        sleep_for(500ms);
        if(!rad.available()) {
            cout << date() << "ERROR: Reply didn't come" << endl;
        }
        while(rad.available()) {
            uint8_t data[PACKET_SIZE];

            rad.read(data, PACKET_SIZE);

            commands::getReply(data, PACKET_SIZE);
        }
    }
}

bool radio::update(time_point &reloadTime) {
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
            reloadTime += (Clock::now() - t) - std::chrono::milliseconds(eeprom::listenTime / 2);

            //((refresh time - listentime) / 2) - (refresh time - (now - t))
            return true;
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
            commands::status::isReporting = true;
        } else {
            lost_packets++;
            commands::status::isReporting = false;
        }
    }

    cout << date() << "< " << (successfull ? "Transmission Received" : "No Transmission available") << endl;

    rad.stopListening();
    rad.powerDown();
    active = false;

    global::light(false);

    return successfull;
}