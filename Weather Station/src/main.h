#ifndef __MAIN__
#define __MAIN__

#define DEBUG
#include "debug.h"

#include <Arduino.h>
#include <avr/wdt.h>


#define LED 13
#define WIND 12
#define RAIN 11

#define BATTPIN A9
#define VAINPIN A0

namespace main {

void set8(uint8_t* index, uint8_t val);

void set16(uint8_t* index, uint16_t val);

void set32(uint8_t* index, uint32_t val);

uint8_t get8(uint8_t* index);

uint16_t get16(uint8_t* index);

uint32_t get32(uint8_t* index);

}

#endif