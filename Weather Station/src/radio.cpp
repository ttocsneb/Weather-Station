#include "radio.h"
#include "eprom.h"
#include "sensors.h"

#include "queue.h"

const byte STATION_ADDRESS[6] = "WTRst";
const byte BASE_ADDRESS[6] = "WTRbs";

uint16_t radio::lostPackets = 0;

struct Command {
    uint8_t* command;
    uint8_t size;
};

QueueArray<Command> commands;

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

void initCommand(uint8_t size, Command &c) {
    c.size = size + 2;
    c.command = new uint8_t[c.size];
    commands.push(c);
}

void getValue(uint8_t* data) {
    #define SET(size, val) c.command[0] = COMMAND_GET_VALUE; \
                           c.command[1] = data[1]; \
                           (size == 1 ? main::set8(c.command + 2, val) : \
                           (size == 2 ? main::set16(c.command + 2, val) : \
                           main::set32(c.command + 2, val)))
    Command c;

    if(*data == COMMAND_GET_VALUE) {
        switch(data[1]) {
        case ALIAS_WIND_TICK:
            initCommand(WIND_TICK_SIZE, c);
            SET(WIND_TICK_SIZE, eeprom::wind::ticks);
            return;
        case ALIAS_WIND_READ_TIME:
            initCommand(WIND_READ_TIME_SIZE, c);
            SET(WIND_READ_TIME_SIZE, eeprom::wind::readTime);
            return;
        case ALIAS_WIND_AVG_UPDATE_TIME:
            initCommand(WIND_AVG_UPDATE_TIME_SIZE, c);
            SET(WIND_AVG_UPDATE_TIME_SIZE, eeprom::wind::averageUpdateTime);
            return;
        case ALIAS_WIND_AVG_STORAGE_TIME:
            initCommand(WIND_AVG_STORAGE_TIME_SIZE, c);
            SET(WIND_AVG_STORAGE_TIME_SIZE, eeprom::wind::averageStorageTime);
            return;
        case ALIAS_PRES_ALTITUDE:
            initCommand(PRES_ALTITUDE_SIZE, c);
            SET(PRES_ALTITUDE_SIZE, eeprom::pressure::altitude);
            return;
        case ALIAS_REFRESH_TIME:
            initCommand(REFRESH_TIME_SIZE, c);
            SET(REFRESH_TIME_SIZE, eeprom::refreshTime);
            return;
        }
    }
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
    return 1;
}

void sendCommands() {
    uint8_t packets[PACKET_SIZE];
    while(!commands.isEmpty()) {
        uint8_t i = 0;

        Command com = commands.front();
        while(!commands.isEmpty() && i + com.size < PACKET_SIZE) {
            com = commands.front();
            uint8_t ind = com.size;
            while(ind--) {
                packets[i + ind] = com.command[ind];
            }
            i += com.size;
            delete[] com.command;
            commands.pop();
        }
        packets[i] = EOT;

        if(!radio::radio.write(packets, PACKET_SIZE)) {
            return;
        }
    }
}

void radio::update() {

    static unsigned long updateTime(0);

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

    static unsigned long replyTime(0);

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
                    } else if(data[i] == COMMAND_GET_VALUE) {
                        getValue(data + i);
                        i++;
                    } else if(data[i] == COMMAND_RESET) {
                        //TODO reset
                    }
                    i++;
                }
                
            }

            if(!commands.isEmpty()) {
                replyTime = millis() + 50;
            }
        }

        if(active && !commands.isEmpty() && millis() > replyTime) {
            radio.stopListening();
            sendCommands();
            radio.startListening();
        }

        //stop listening for commands and power down after [LISTEN_TIME] ms
        if(active && millis() - updateTime > LISTEN_TIME) {
            active = false;
            radio.stopListening();
            radio.powerDown();
        }
    }

}

