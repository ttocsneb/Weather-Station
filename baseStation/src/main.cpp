#include <iostream>
#include <sstream>
#include <iomanip>
#include <wiringPi.h>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

#ifndef _WIN32
#include <systemd/sd-daemon.h>
#endif


#include "main.h"
#include "radio.h"
#include "commands.h"


using std::cout;
using std::endl;

#define LIGHT_PIN 27

void global::begin() {
    pinMode(LIGHT_PIN, OUTPUT);
}

bool lightValue = false;

void global::light(bool value) {
    if(lightValue != value) {
        digitalWrite(LIGHT_PIN, value);
        lightValue = value;
    }
}

void global::toggleLight() {
    lightValue = !lightValue;
    digitalWrite(LIGHT_PIN, lightValue);
}

std::string global::getsqlDate(std::chrono::system_clock::time_point t) {
    std::stringstream ss;

    time_t tt = system_clock::to_time_t(t);
    struct std::tm * ptm = std::localtime(&tt);

    ss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}


void global::setBool(uint8_t* index, uint8_t location, bool val) {
    if(val) {
        *index = *index | (1 << location);
    } else {
        *index = *index & ~(1 << location);
    }
}

void set8(uint8_t* index, uint8_t val) {
    *index = val;
}

void set16(uint8_t* index, uint16_t val) {
    set8(index, (val >> 8) & 0xFF);
    set8(index + 1, val & 0xFF);
}

void set32(uint8_t* index, uint32_t val) {
    set16(index, (val >> 16) & 0xFFFF);
    set16(index + 2, val & 0xFFFF);
}

bool global::getBool(const uint8_t* index, uint8_t location) {
    return (*index >> location) & 1;
}

uint8_t get8(const uint8_t* index) {
    return *index;
}

uint16_t get16(const uint8_t* index) {
    return (static_cast<uint16_t>(get8(index)) << 8) | get8(index + 1);
}

uint32_t get32(const uint8_t* index) {
    return (static_cast<uint32_t>(get16(index)) << 16) | get16(index + 2);
}

namespace global {

    template<> uint8_t get<uint8_t>(const uint8_t* index) {
        return get8(index);
    }
    template<> uint16_t get<uint16_t>(const uint8_t* index) {
        return get16(index);
    }
    template<> uint32_t get<uint32_t>(const uint8_t* index) {
        return get32(index);
    }


    template<> void set<uint8_t>(uint8_t* index, uint8_t val) {
        set8(index, val);
    }
    template<> void set<uint16_t>(uint8_t* index, uint16_t val) {
        set16(index, val);
    }
    template<> void set<uint32_t>(uint8_t* index, uint32_t val) {
        set32(index, val);
    }
}