#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

#define PACKET_SIZE 32

#define EOT '\0'

//EEPROM Commands

#define COMMAND_SET_VALUE 'S'
#define COMMAND_GET_VALUE 'G'
#define COMMAND_SET_EEPROM 'E'
#define COMMAND_LOAD_EEPROM 'L'
#define COMMAND_RESET 'R'

//EEPROM SET/GET ALIASES

#define ALIAS_WIND_TICK 't'
#define ALIAS_WIND_READ_TIME 'r'

#define ALIAS_WIND_AVG_UPDATE_TIME 'u'
#define ALIAS_WIND_AVG_STORAGE_TIME 's'

#define ALIAS_PRES_ALTITUDE 'a'

#define ALIAS_REFRESH_TIME 'R'

enum EEPROM_Variable {
    WIND_TICK='t',WIND_READ_TIME='r',WIND_AVG_UPDATE_TIME='u',WIND_AVG_STORAGE_TIME='s',
    PRES_ALTITUDE='a',REFRESH_TIME='r'
};

//Packet Value locations

#define WIND_SPEED_LOC 0
#define WIND_DIREC_LOC 2
#define WIND_MAX_SPEED_LOC 4
#define WIND_MAX_DIREC_LOC 6
#define WIND_AVG_SPEED_LOC 8
#define WIND_AVG_DIREC_LOC 10

#define HUMIDITY_LOC 12
#define TEMPERATURE_LOC 14
#define RAIN_LOC 16
#define PRESSURE_LOC 17


namespace radio {


    void begin();

    bool update();

    template<typename T>
    void setEEPROM(EEPROM_Variable variable, T value);

        
    extern uint16_t rawWindSpeed;
    extern uint16_t rawWindDirection;
    extern uint16_t rawMaxWindSpeed;
    extern uint16_t rawMaxWindDirection;
    extern uint16_t rawAverageWindSpeed;
    extern uint16_t rawAverageWindDirection;

    extern uint16_t rawHumidity;
    extern int16_t rawTemperature;
    extern uint8_t rawRainFall;
    extern uint16_t rawPressure;
}

#endif