#ifndef __SENSORS__
#define __SENSORS__

#include "main.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 5

#include <FaBoBarometer_MPL115.h>
#include <Wire.h>

namespace sensors {

extern uint16_t maxWind;
extern uint16_t maxDirection;

extern uint16_t averageWind;
extern uint16_t averageDirection;


extern uint8_t rain;

extern DHT dht;
extern FaBoBarometer baro;

void begin();

void update();

void loadPacket(uint8_t* packet);

uint16_t getWindSpeed();
uint16_t getWindDirection();
uint16_t getMaxWindSpeed();
uint16_t getMaxWindDirection();
uint16_t getAverageWindSpeed();
uint16_t getAverageWindDirection();

uint16_t getHumidity();
int16_t getTemperature();
int8_t getRainFall();
uint16_t getPressure();


}

#endif