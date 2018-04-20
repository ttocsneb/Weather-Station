#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>
#include <cstddef>

#define PACKET_SIZE 32

#define EOT '\0'

//EEPROM Commands

#define COMMAND_SET_VALUE 'S'
#define COMMAND_GET_VALUE 'G'
#define COMMAND_GET_STATUS 'U'
#define COMMAND_SET_EEPROM 'E'
#define COMMAND_LOAD_EEPROM 'L'
#define COMMAND_RESET 'R'


enum EEPROM_Variable {
    WIND_TICK='t',WIND_READ_TIME='r',WIND_AVG_UPDATE_TIME='u',WIND_AVG_STORAGE_TIME='s',
    PRES_ALTITUDE='a',REFRESH_TIME='R'
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

    typedef void (*EEPROM_got)(EEPROM_Variable);
    void getEEPROM(EEPROM_Variable variable, EEPROM_got success=NULL);

    typedef void (*Status_got)();
    void getStatus(Status_got success=NULL);

    
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

    namespace status {
        extern uint16_t lostPackets;
        extern uint16_t chargingTime;
        extern bool isCharging;
        extern float batteryVoltage;
        extern uint8_t numResets;
    }
}

#endif