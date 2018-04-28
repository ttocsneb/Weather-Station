#ifndef EPROM_H
#define EPROM_H


#include <stdint.h>

namespace eeprom {

    extern uint32_t refreshTime;
    extern uint32_t listenTime;

    extern float windConversion;

    extern unsigned int dailyRain;

    /**
     * Read EEPROM from file
     */
    void loadEEPROM();
    /**
     * Sync EEPROM with WeatherStation
     * 
     * @note prefers values from BaseStation
     */
    void syncEEPROM();
    /**
     * Save current values to EEPROM
     */
    void setEEPROM();
}

#endif