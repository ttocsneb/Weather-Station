#ifndef RADIO_H
#define RADIO_H

#include <stdint.h>
#include <cstddef>

#include "commands.h"
#include "main.h"

#define PACKET_SIZE 32

#define EOT '\0'

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

    /**
     * Initialize the radio namespace
     */
    void begin();

    /**
     * Update the raw weather data with the station.
     * 
     * @note t may be changed if the loop time needs adjustment
     * 
     * @param t The time at which the main loop will wait for
     * 
     * @note should only be called once every 30 seconds
     */
    bool update(time_point &t);

    
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