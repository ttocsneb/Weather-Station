#include "sensors.h"
#include "radio.h"
#include "eprom.h"

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


/**
 * Set an eeprom value with the given data array
 */
void setValue(uint8_t* data) {
    long value = main::get32(data + 1);

    switch(*data) {
    case REFRESH_TIME:
        eeprom::refresh_time = value;
    case LISTEN_TIME:
        eeprom::listen_time = value;
    case ALTITUDE:
        eeprom::pressure::altitude = value;
    case AVG_WIND_UPDATE:
        eeprom::wind::average_update = value;
    case AVG_WIND_STORAGE:
        eeprom::wind::average_storage = value;
    case WIND_UPDATE:
        eeprom::wind::update = value;
    case WIND_STORAGE:
        eeprom::wind::storage = value;
    }
}


void sendCommands() {
    while(!commands.isEmpty()) {
        uint8_t i = 0;

        Command com;
        while(!commands.isEmpty() && i + com.size < PACKET_SIZE) {
            com = commands.front();

            // Send the command to the basestation
            if(!radio::radio.write(com.command, com.size)) {
                // If the command failed to send, don't send anymore,
                // and don't delete the failed command
                return;
            }

            // Remove the Command
            delete[] com.command;
            commands.pop();
        }
    }
}


void radio::update() {

    static unsigned long updateTime(0);

    unsigned long current_time = millis();

    //send an update to the base station every [eeprom::refresh_time] ms
    if(current_time - updateTime > eeprom::refresh_time) {
        updateTime = current_time;

        digitalWrite(LED, HIGH);

        //power up the radio
        active = true;
        radio.powerUp();

        //load the packet
        uint8_t data[PACKET_SIZE];
        data[0] = COMMAND_GET_WEATHER;
        sensors::loadPacket(data + 1);

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

                uint8_t command = data[0];

                if(command == COMMAND_GET_WEATHER) {
                    DEBUG_PRINTLN("Got Command get_weather");

                    Command c;
                    c.size = 17;
                    c.command = new uint8_t[c.size];
                    c.command[0] = COMMAND_GET_WEATHER;
                    sensors::loadPacket(c.command + 1);
                    commands.push(c);
                } else if(command == COMMAND_GET_SETTINGS) {
                    DEBUG_PRINTLN("Got Command get_settings");

                    Command c;
                    c.size = 19;
                    c.command = new uint8_t[c.size];
                    c.command[0] = COMMAND_GET_SETTINGS;
                    eeprom::loadPacket(c.command + 1);
                    commands.push(c);
                    // TODO: Implement Get Settings Command
                } else if(command == COMMAND_GET_STATUS) {
                    DEBUG_PRINTLN("Got Command get_status");

                    Command c;
                    c.size = 6;
                    c.command = new uint8_t[c.size];
                    c.command[0] = COMMAND_GET_STATUS;
                    main::loadStatus(c.command + 1);
                    commands.push(c);
                } else if(command == COMMAND_SET_SETTINGS) {
                    DEBUG_PRINTLN("Got Command set_settings");

                    setValue(data + 1);
                    eeprom::setEEPROM();
                } else if (command == COMMAND_RESET) {
                    DEBUG_PRINTLN("Got Command reset");

                    sensors::reset();
                } else if (command == COMMAND_RESET_STATUS) {
                    DEBUG_PRINTLN("Got Command reset_status");
                    main::reset();
                } else if (command == COMMAND_RESET_SETTINGS) {
                    DEBUG_PRINTLN("Got Command reset_settings");
                    eeprom::reset();
                } else if (command == COMMAND_RESTART) {
                    DEBUG_PRINTLN("Got Command reset_restart");
                    main::restart();
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
        if(active && millis() - updateTime > eeprom::listen_time) {
            active = false;
            radio.stopListening();
            radio.powerDown();
        }
    }

}

