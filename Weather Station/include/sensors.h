#ifndef __SENSORS__
#define __SENSORS__

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <FaBoBarometer_MPL115.h>
#include <Wire.h>

#include "main.h"

#define DHTTYPE DHT22
#define DHTPIN 5


#define WIND_DIREC_LOC 0
#define WIND_MAX_DIREC_LOC 1
#define WIND_AVG_DIREC_LOC 2
#define WIND_SPEED_LOC 4
#define WIND_MAX_SPEED_LOC 6
#define WIND_AVG_SPEED_LOC 8

#define RAIN_LOC 10
#define TEMPERATURE_LOC 11
#define HUMIDITY_LOC 13
#define PRESSURE_LOC 15

namespace sensors {

    extern uint16_t maxWind;
    extern uint16_t maxDirection;

    extern uint16_t averageWind;
    extern uint16_t averageDirection;


    extern volatile uint8_t rain;

    extern DHT dht;
    extern FaBoBarometer baro;

    void begin();

    void update();

    void loadPacket(uint8_t* packet);
    void reset();

    uint8_t getWindSpeed();
    uint16_t getWindDirection();
    uint16_t getMaxWindSpeed();
    uint16_t getMaxWindDirection();
    uint16_t getAverageWindSpeed();
    uint16_t getAverageWindDirection();

    uint16_t getHumidity();
    int16_t getTemperature();
    uint8_t getRainFall();
    uint16_t getPressure();


}

#endif