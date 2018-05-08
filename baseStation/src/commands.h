#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <cstddef>


#define WIND_TICK_TYPE uint8_t
#define WIND_READ_TIME_TYPE uint16_t
#define WIND_AVG_UPDATE_TIME_TYPE uint16_t
#define WIND_AVG_STORAGE_TIME_TYPE uint32_t
#define PRES_ALTITUDE_TYPE uint16_t
#define REFRESH_TIME_TYPE uint32_t



/**
 * EEPROM Variables that can be set
 * 
 * The value of each enum is the alias used
 */
enum EEPROM_Variable {
    ALIAS_WIND_TICK='t',
    ALIAS_WIND_READ_TIME='r',
    ALIAS_WIND_AVG_UPDATE_TIME='u',
    ALIAS_WIND_AVG_STORAGE_TIME='s',
    ALIAS_PRES_ALTITUDE='a',
    ALIAS_REFRESH_TIME='R'
};

/**
 * Command Aliases
 * 
 * The value of each enum is the byte that is sent
 */
enum COMMANDS {
    COMMAND_SET_VALUE='S',
    COMMAND_GET_VALUE='G',
    COMMAND_GET_STATUS='U',
    COMMAND_SET_EEPROM='E',
    COMMAND_LOAD_EEPROM='L',
    COMMAND_RESET='R'
};


namespace commands {

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

        extern uint32_t refreshTime;

    }

    namespace status {
        extern uint16_t lostPackets;
        extern uint16_t chargingTime;
        extern bool isCharging;
        extern float batteryVoltage;
        extern uint8_t numResets;
        extern uint32_t uptime;
        extern bool isReporting;
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
    typedef void (*EEPROM_Got)(EEPROM_Variable);
    /**
     * Get the value of an EEPROM variable
     * 
     * @param variable variable to get
     * @param success function pointer to call when the EEPROM 
     * variable is received
     */
    void getEEPROM(EEPROM_Variable variable, EEPROM_Got success=NULL);

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
     * Save the currently set values to EEPROM
     */
    void saveEEPROM();
    /**
     * Load the values from EEPROM, resetting anything changed 
     * since last save
     */
    void loadEEPROM();

    /**
     * Load the commands into a packet to send to the weather station
     * 
     * @param packet packet
     * @param size size of packet
     * 
     * @returns true if a reply is expected from the weather station
     */
    bool loadCommands(uint8_t* packet, uint8_t size);
    /**
     * Load the receiving commands
     * 
     * @param packet packet
     * @param size size of packet
     */
    void getReply(const uint8_t* packet, uint8_t size);

    /**
     * Parse the commands file .commands and execute them
     */
    void parseCommandsFile();
}

#endif