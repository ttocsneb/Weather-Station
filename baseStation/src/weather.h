#ifndef WEATHER_H
#define WEATHER_H

#include <stdint.h>

namespace weather {

    extern float windSpeed;
    extern uint16_t windDirection;
    extern float windGust10Min;
    extern uint16_t windGustDirection10Min;
    extern float averageWindSpeed2Min;
    extern uint16_t averageWindDirection2Min;

    extern float humidity;
    extern float dewpoint;
    extern float temperature;
    extern float barometer;

    extern float rainHour;
    extern float rainDay;

    void update();
    void begin();
}

#endif