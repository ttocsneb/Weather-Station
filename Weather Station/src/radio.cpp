#include "radio.h"
#include "eprom.h"
#include "sensors.h"

const byte STATION_ADDRESS[6] = "WTRst";
const byte BASE_ADDRESS[6] = "WTRbs";

uint16_t radio::lostPackets = 0;

RF24 radio::radio(6, 10);

bool active;

void radio::begin() {
    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setRetries(15, 15);
    radio.setPayloadSize(PACKET_SIZE);

    radio.openWritingPipe(BASE_ADDRESS);
    radio.openReadingPipe(1, STATION_ADDRESS);

    radio.powerDown();

    active = false;
}

void radio::update() {

    static long updateTime(0);

    //send an update to the base station every [eeprom::refreshTime] ms
    if(millis() - updateTime > eeprom::refreshTime) {
        updateTime = millis();

        digitalWrite(LED, HIGH);

        //power up the radio
        active = true;
        radio.powerUp();

        //load the packet
        uint8_t data[PACKET_SIZE];
        sensors::loadPacket(data);

        //send the packet to the base station, if it fails, try [RETRIES] more times
        bool success = false;
        uint8_t i = RETRIES;
        while(i-- && !success) {
            success = radio.write(data, PACKET_SIZE);
            delay(50);
        }
        //add 1 to lostpackets if the packet couldn't be sent
        if(success) {
            sensors::reset();
        } else {
            lostPackets++;
        }

        //start listening for commands from the base station
        radio.startListening();

        digitalWrite(LED, LOW);
    }

    //if the radio is active, we should check for incoming commands
    if(active) {

        if(radio.available()) {
            //TODO read commands from the base station
        }

        if(millis() - updateTime > LISTEN_TIME) {
            active = false;
            radio.stopListening();
            radio.powerDown();
        }
    }

}

