#ifndef EPROM_H
#define EPROM_H


#include <stdint.h>

namespace eeprom {

    namespace station {
        namespace wind {
            extern uint8_t ticks;
            extern uint16_t readTime;

            extern uint16_t averageUpdateTime;
            extern uint32_t averageStorageTime;
        }

        namespace pressure {
            extern uint16_t altitude;
        }

    }

    extern uint32_t refreshTime;
    extern uint32_t listenTime;

    extern float windConversion;

    extern unsigned int dailyRain;

    void loadEEPROM();
    void syncEEPROM();
    void setEEPROM();
}

#endif