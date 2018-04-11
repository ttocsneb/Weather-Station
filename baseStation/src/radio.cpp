#include "radio.h"
#include "main.h"

#include <iostream>
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

void radio::begin() {
    global::light(true);
    cout << date() << " > Begin Radio" << endl;

    rad.begin();

    rad.setDataRate(RF24_250KBPS);
    rad.setRetries(15, 15);
    rad.setPayloadSize(PACKET_SIZE);
    
    rad.openWritingPipe(STATION_ADDRESS);
    rad.openReadingPipe(1, BASE_ADDRESS);

    rad.powerDown();
    active = false;

    rad.printDetails();

    cout << date() << " < Begin Radio Complete" << endl;
    global::light(false);
}

bool checkForPacket() {
    if(active) {
        //TODO check for available packets
    }

    return false;
}

void radio::update() {
    static unsigned int lost_packets(0);
    bool successfull = false;

    global::light(true);

    cout << date() << " > Waiting for transmission" << endl;

    active = true;
    rad.powerUp();

    //check if the timer is out of sync with the station
    if(lost_packets > 2) {
        cout << date() << "Too many lost packets, possible sync error.  Resyncing" << endl;

        lost_packets = 0;

        time_point t =  Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < CYCLE_TIME) {
            sleep_for(1s);
            global::toggleLight();
            successfull = checkForPacket();
        }
        global::light(true);


        active = false;
        rad.powerDown();

        if(successfull) {
            cout << date() << "Packet received, waiting " << (CYCLE_TIME - LISTEN_TIME / 2) / 1000.0 << " seconds to sync with the station" << endl;
            sleep_for(std::chrono::milliseconds(CYCLE_TIME - LISTEN_TIME / 2));
        }
    } else {

        time_point t = Clock::now();
        while(!successfull && timeDiff(t,  Clock::now()) < LISTEN_TIME) {
            sleep_for(500ms);
            global::toggleLight();
            successfull = checkForPacket();
        }
        global::light(true);

        if(successfull) {
            lost_packets = std::min(lost_packets - 1, (unsigned int)(0));
        } else {
            lost_packets++;
        }
    }

    cout << date() << " < " << (successfull ? "Transmission Received" : "No Transmission available") << endl;

    if(active) {
        rad.powerDown();
        active = false;
    }

    global::light(false);
}