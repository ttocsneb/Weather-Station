#ifndef EPROM_H
#define EPROM_H


#include <stdint.h>

namespace eeprom {

    extern uint32_t refreshTime;
    extern uint32_t listenTime;

    void loadEEPROM();
    void syncEEPROM();
    void setEEPROM();
}

#endif