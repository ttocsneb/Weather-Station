#ifndef __MAIN__
#define __MAIN__

//#define DEBUG
#include "debug.h"

#include <Arduino.h>


#define LED 13
#define WIND 12
#define RAIN 11

#define BATTPIN A9
#define VAINPIN A0

#define SOLARVOLTAGEPIN A1
#define SOLARENABLEPIN 20
#define BATTERY_POLL_TIME 5000

namespace main {

    void setB(uint8_t* index, uint8_t location, bool val);
    void set8(uint8_t* index, uint8_t val);
    void set16(uint8_t* index, uint16_t val);
    void set32(uint8_t* index, uint32_t val);

    bool getB(uint8_t* index, uint8_t location);
    uint8_t get8(uint8_t* index);
    uint16_t get16(uint8_t* index);
    uint32_t get32(uint8_t* index);

    float getSolarVoltage();

    float getBatteryVoltage();

    float getBatteryTemperature();

    extern bool isCharging;
    extern bool justStarted;
    
    

    void loadStatus(uint8_t* data);

    void updateBattery();

    void restart();

    void reset();

}

#endif