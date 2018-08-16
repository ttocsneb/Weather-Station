#ifndef EPROM_H
#define EPROM_H


#include <stdint.h>
#include <string>

namespace eeprom {

    extern uint32_t refreshTime;
    extern uint32_t listenTime;

    extern uint16_t resets;

    namespace weather {

        extern float windConversion;

        extern unsigned int dailyRain;

    }

    namespace params {
        extern std::string database;
    }

    namespace sql {
        extern unsigned int weatherData_storageTime;
    }



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