#ifndef __EPROM__
#define __EPROM__

#include <Arduino.h>

//EEPROM Locations

#define REFRESH_TIME 0xA1
#define LISTEN_TIME 0x17

#define ALTITUDE 0xC0

#define AVG_WIND_UPDATE 0xD0
#define AVG_WIND_STORAGE 0xD3

#define WIND_UPDATE 0xDA
#define WIND_STORAGE 0xDD

namespace eeprom {
    namespace wind {
        // The time between averaging polls
        extern uint16_t average_update;
        // The length of the average, in ms
        extern uint32_t average_storage;
        extern uint8_t average_queue_size;

        // The time between wind polls
        extern uint16_t update;
        // The length of the wind measurement, in ms
        extern uint16_t storage;
        extern uint8_t queue_size;
    }

    namespace pressure {
        // The altitude of the station (m)
        extern uint16_t altitude;
    }

    // The time in ms between each weather transmission
    extern uint32_t refresh_time;

    // The time to wait before a response in ms
    extern uint16_t listen_time;

    // The value of the shortest delay
    extern uint16_t shortest_delay;

    void loadEEPROM();

    void setEEPROM();
    
    void loadPacket(uint8_t* packet);

    void reset();
}

#endif