#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>

#define PACKET_SIZE 32

#define EOT '\0'

#define COMMAND_SET_VALUE 'S'
#define COMMAND_SET_EEPROM 'E'
#define COMMAND_LOAD_EEPROM 'L'
#define COMMAND_RESET 'R'


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

    void update();

        
    float getWindSpeed();
    uint16_t getWindDirection();
    float getMaxWindSpeed();
    uint16_t getMaxWindDirection();
    float getAverageWindSpeed();
    uint16_t getAverageWindDirection();

    float getHumidity();
    float getTemperature();
    float getRainFall();
    float getPressure();
}

#endif