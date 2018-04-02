#ifndef __EPROM__
#define __EPROM__

#include <Arduino.h>

#define WIND_TICK 0
#define WIND_READ_TIME 1

#define WIND_AVG_UPDATE_TIME 4
#define WIND_AVG_STORAGE_TIME 6

#define PRES_ALTITUDE 10

#define REFRESH_TIME 12

#define RESETS 3

namespace eeprom {
namespace wind {
extern uint8_t ticks;
extern uint16_t readTime;
extern uint16_t tickDelay;
extern uint16_t tickMultiplier;

extern uint16_t averageUpdateTime;
extern uint32_t averageStorageTime;
extern uint8_t averageTimeN;
}

namespace pressure {
extern uint16_t altitude;
}

extern uint8_t resets;

extern uint32_t refreshTime;

void loadEEPROM();

void setEEPROM();
}




#endif