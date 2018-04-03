#ifndef __EPROM__
#define __EPROM__

#include <Arduino.h>

//EEPROM Locations

#define WIND_TICK 0
#define WIND_TICK_SIZE 1
#define WIND_READ_TIME (WIND_TICK + WIND_TICK_SIZE)
#define WIND_READ_TIME_SIZE 2

#define WIND_AVG_UPDATE_TIME (WIND_READ_TIME + WIND_READ_TIME_SIZE)
#define WIND_AVG_UPDATE_TIME_SIZE 2
#define WIND_AVG_STORAGE_TIME (WIND_AVG_UPDATE_TIME + WIND_AVG_UPDATE_TIME_SIZE)
#define WIND_AVG_STORAGE_TIME_SIZE 4

#define PRES_ALTITUDE (WIND_AVG_STORAGE_TIME + WIND_AVG_STORAGE_TIME_SIZE)
#define PRES_ALTITUDE_SIZE 2

#define REFRESH_TIME (PRES_ALTITUDE + PRES_ALTITUDE_SIZE)
#define REFRESH_TIME_SIZE 4

#define RESETS (REFRESH_TIME + REFRESH_TIME_SIZE)
#define RESETS_SIZE 1

//EEPROM ALIAS

#define ALIAS_WIND_TICK 't'
#define ALIAS_WIND_READ_TIME 'r'

#define ALIAS_WIND_AVG_UPDATE_TIME 'u'
#define ALIAS_WIND_AVG_STORAGE_TIME 's'

#define ALIAS_PRES_ALTITUDE 'a'

#define ALIAS_REFRESH_TIME 'R'

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