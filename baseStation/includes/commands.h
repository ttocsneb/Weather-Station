#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <cstddef>



#define TYPE_REFRESH_TIME uint32_t
#define TYPE_LISTEN_TIME uint16_t
#define TYPE_ALTITUDE uint16_t
#define TYPE_AVG_WIND_UPDATE uint16_t
#define TYPE_AVG_WIND_STORAGE uint32_t
#define TYPE_WIND_UPDATE uint16_t
#define TYPE_WIND_STORAGE uint16_t



/**
 * EEPROM Variables that can be set
 * 
 * The value of each enum is the alias used
 */
enum EEPROM_Variable {
    EEPROM_REFRESH_TIME=0xA1,
    EEPROM_LISTEN_TIME=0x17,

    EEPROM_ALTITUDE=0xC0,

    EEPROM_AVG_WIND_UPDATE=0xD0,
    EEPROM_AVG_WIND_STORAGE=0xD3,
    EEPROM_WIND_UPDATE=0xDA,
    EEPROM_WIND_STORAGE=0xDD
};

/**
 * Command Aliases
 * 
 * The value of each enum is the byte that is sent
 */
enum COMMANDS {
    COMMAND_GET_WEATHER=0xA0,
    COMMAND_GET_SETTINGS=0xA5,
    COMMAND_GET_STATUS=0xAB,

    COMMAND_RESET=0xB0,
    COMMAND_RESET_STATUS=0xB3,

    COMMAND_RESET_SETTINGS=0xC0,
    COMMAND_RESTART=0xCA,

    COMMAND_SET_SETTINGS=0xD5
};


namespace commands {

    namespace eeprom {
        namespace wind {
            extern TYPE_WIND_UPDATE wind_update;
            extern TYPE_WIND_STORAGE wind_storage;

            extern TYPE_AVG_WIND_UPDATE avg_wind_update;
            extern TYPE_AVG_WIND_STORAGE avg_wind_storage;
        }

        namespace pressure {
            extern TYPE_ALTITUDE altitude;
        }

        extern TYPE_REFRESH_TIME refresh_time;
        extern TYPE_LISTEN_TIME listen_time;
    }

    namespace status {
        extern bool just_started;
        extern bool charging;
        extern float battery;
        extern float battery_temp;
        extern uint8_t lost_packets;
        extern bool isReporting;
    }

    namespace weather {
        extern uint16_t rawWindSpeed;
        extern uint8_t rawWindDirection;
        extern uint16_t rawMaxWindSpeed;
        extern uint8_t rawMaxWindDirection;
        extern uint16_t rawAverageWindSpeed;
        extern uint16_t rawAverageWindDirection;

        extern uint8_t rawHumidity;
        extern int16_t rawTemperature;
        extern uint8_t rawRainFall;
        extern uint16_t rawPressure;
    }

    /**
     * Set an EEPROM variable on the weatherstation
     * 
     * @note type T should be determined by X_TYPE definition
     * 
     * @code
     * setEEPROM<WIND_TICK_TYPE>(ALIAS_WIND_TICK, x);
     * @endcode
     * 
     * @param variable Variable to set
     * @param value value of variable
     */
    template<typename T>
    void setEEPROM(EEPROM_Variable variable, T value);


    /**
     * Function pointer that is called when an eeprom variable is received
     * 
     * Used in getEEPROM()
     */
    typedef void (*EEPROM_Got)();
    /**
     * Get the value of an EEPROM variable
     * 
     * @param success function pointer to call when the EEPROM 
     * variable is received
     */
    void getEEPROM(EEPROM_Got success=NULL);

    /**
     * Function pointer that is called when the status is received
     * 
     * Used in getStatus()
     */
    typedef void (*Status_got)();
    /**
     * Get the status of the weather station
     * 
     * @param success function pointer to call when the status is received
     */
    void getStatus(Status_got success=NULL);

    /**
     * Reset the weather counters. (rain, max_wind, etc.)
     */
    void resetWeather();
    /**
     * Reset the Status counters. (lost_packets, just_started, etc.)
     */
    void resetStatus();

    /**
     * Let the watchdog reset the weatherstation
     */
    void restart();

    /**
     * Reset the eeprom to its defaults
     */
    void resetEEPROM();

    /**
     * Load the commands into a packet to send to the weather station
     * 
     * @param packet packet
     * @param size size of packet
     * 
     * @returns true if a reply is expected from the weather station
     */
    bool loadCommand(uint8_t* packet, uint8_t size);
    /**
     * Return true if there are more commands to be processed.
     */
    bool moreCommands();

    /**
     * Load the receiving commands
     * 
     * @param packet packet
     * @param size size of packet
     */
    void getReply(const uint8_t* packet, uint8_t size);

}

#endif