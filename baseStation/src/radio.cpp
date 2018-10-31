#include "radio.h"
#include "main.h"
#include "commands.h"

#include <iostream>
#include <queue>
#include <algorithm>
#include <RF24/RF24.h>

using std::cout; 
using std::endl;
using std::cerr;

const uint8_t STATION_ADDRESS[6] = "WTRst";
const uint8_t BASE_ADDRESS[6] = "WTRbs";

#define CE_PIN 25
#define CS_PIN 0

RF24 rad(CE_PIN, CS_PIN);

bool active;

void radio::begin() {
    global::light(true);
    cout << "> Begin Radio" << endl;

    rad.begin();

    rad.setDataRate(RF24_250KBPS);
    rad.setRetries(15, 15);
    rad.setPayloadSize(PACKET_SIZE);
    
    rad.openWritingPipe(STATION_ADDRESS);
    rad.openReadingPipe(1, BASE_ADDRESS);

    rad.powerDown();
    active = false;

    rad.printDetails();

    cout << "< Begin Radio Complete" << endl;
    global::light(false);
}

bool checkForPacket() {
    D(cout << "Checking for Packet" << endl);
    if(active) {
        if(rad.available()) {
            D(cout << "Packet received" << endl);
            uint8_t data[PACKET_SIZE];

            rad.read(data, PACKET_SIZE);

            commands::getReply(data, PACKET_SIZE);

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

    bool expectResponse = false;

    while(commands::moreCommands()) {
        bool expectResponse = commands::loadCommand(packets, PACKET_SIZE) || expectResponse;

        if(!rad.write(packets, PACKET_SIZE)) {
            cerr << "ERROR: Could not send Command" << endl;
            return expectResponse;
        }
    }

    return expectResponse;
}

void sendCommands() {
    rad.stopListening();
    if(sendPackets()) {
        D(cout << "Expecting Reply" << endl);
        rad.startListening();
        sleep_for(500ms);
        if(!rad.available()) {
            D(cout << "ERROR: Reply didn't come" << endl);
        }
        while(rad.available()) {
            uint8_t data[PACKET_SIZE];

            rad.read(data, PACKET_SIZE);

            commands::getReply(data, PACKET_SIZE);
        }
    }
}

int radio::update() {
    static unsigned int lost_packets(100);
    bool successfull = false;

    int reloadTime = 0;

    global::light(true);

    D(cout << "> Waiting for transmission" << endl);

    active = true;
    rad.powerUp();
    rad.startListening();

    //check if the timer is out of sync with the station
    if(lost_packets > 1) {
        cout << "Too many lost packets, possible sync error.  Resyncing" << endl;

        lost_packets = 0;

        //wait for up to commands::eeprom::refreshTime (30s) for an incomming packet
        time_point t =  Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < commands::eeprom::refreshTime) {
            sleep_for(500ms);
            global::toggleLight();
            successfull = checkForPacket();
        }
        global::light(true);

        
        //if successful, try to send the commands
        if(successfull) {
            lost_packets = std::min(lost_packets - 1, (unsigned int)(0));
            sendCommands();
            commands::status::isReporting = true;
        } else {
            lost_packets++;
            commands::status::isReporting = false;
        }

        active = false;
        rad.stopListening();
        rad.powerDown();

        //if a packet was received, wait until the next update time.
        if(successfull) {
            D(cout << "Packet received, waiting " << (commands::eeprom::refreshTime - commands::eeprom::listenTime / 2) / 1000.0 << " seconds to sync with the station" << endl);
            reloadTime = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now() - t) - commands::eeprom::listenTime / 2;

            //((refresh time - listentime) / 2) - (refresh time - (now - t))
            return reloadTime;
        }
        return -1;
    } else {

        int8_t count = 0;

        //wait for an available packet
        time_point t = Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < commands::eeprom::listenTime) {
            sleep_for(500ms);
            successfull = checkForPacket();
            count++;
        }

        //If we are desyncing, compensate.
        count -= (commands::eeprom::listenTime / 500) / 2;
        if(count != 0) {
            reloadTime = (count * 500ms);
            cout << "Desynced by " << count * 500 << "ms" << endl;
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

    D(cout << "< " << (successfull ? "Transmission Received" : "No Transmission available") << endl);

    rad.stopListening();
    rad.powerDown();
    active = false;

    global::light(false);

    return successfull ? reloadTime : -1;
}