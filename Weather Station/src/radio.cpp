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


/**
 * Set an eeprom value with the given data array
 *
 * @return number of bytes used
 */
uint8_t setValue(uint8_t* data) {
#define getX(x, y) (x == 1 ? main::get8(y) : (x == 2 ? main::get16(y) : main::get32(y)))

    switch(*data) {
    case ALIAS_WIND_TICK:
        eeprom::wind::ticks = getX(WIND_TICK_SIZE, data + 1);
        return WIND_TICK_SIZE + 1;
    case ALIAS_WIND_READ_TIME:
        eeprom::wind::readTime = getX(WIND_READ_TIME_SIZE, data + 1);
        return WIND_READ_TIME_SIZE + 1;
    case ALIAS_WIND_AVG_UPDATE_TIME:
        eeprom::wind::averageUpdateTime = getX(WIND_AVG_UPDATE_TIME_SIZE, data + 1);
        return WIND_AVG_UPDATE_TIME_SIZE + 1;
    case ALIAS_WIND_AVG_STORAGE_TIME:
        eeprom::wind::averageStorageTime = getX(WIND_AVG_STORAGE_TIME_SIZE, data + 1);
        return WIND_AVG_STORAGE_TIME_SIZE + 1;

    case ALIAS_PRES_ALTITUDE:
        eeprom::pressure::altitude = getX(PRES_ALTITUDE_SIZE, data + 1);
        return PRES_ALTITUDE_SIZE + 1;

    case ALIAS_REFRESH_TIME:
        eeprom::refreshTime = getX(REFRESH_TIME_SIZE, data + 1);
        return REFRESH_TIME_SIZE + 1;
    }
    return 0;
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
            uint8_t data[PACKET_SIZE];

            //read every availabe packet
            while(radio.available()) {
                radio.read(data, PACKET_SIZE);

                //commands can be nested in packets, process each command

                uint8_t i = 0;
                while(data[i] != EOT && i < PACKET_SIZE) {
                    if(data[i] == COMMAND_SET_VALUE) {
                        i += setValue(data + i + 1);
                    } else if(data[i] == COMMAND_SET_EEPROM) {
                        eeprom::setEEPROM();
                    } else if(data[i] == COMMAND_LOAD_EEPROM) {
                        eeprom::loadEEPROM();
                    } else if(data[i] == COMMAND_RESET) {
                        //TODO reset
                    }
                    i++;
                }
            }
        }

        //stop listening for commands and power down after [LISTEN_TIME] ms
        if(millis() - updateTime > LISTEN_TIME) {
            active = false;
            radio.stopListening();
            radio.powerDown();
        }
    }

}

