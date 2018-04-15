#include <iostream>
#include <sstream>
#include <iomanip>
#include <wiringPi.h>

#include "main.h"
#include "radio.h"
#include "eprom.h"
#include "weather.h"


using std::cout;
using std::endl;

int main(int argc, char** argv) {
    wiringPiSetup();

    pinMode(27, OUTPUT);

    eeprom::loadEEPROM();
    eeprom::setEEPROM();

    weather::begin();

    radio::begin();


    while(true) {
        time_point t = system_clock::now() + 30s;
        if(radio::update()) {
            weather::update();
        }
        sleep_until(t);
    }

    return 0;
}

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

std::string date() {
    std::stringstream ss;

    time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm * ptm = std::localtime(&tt);

    ss << "[" << std::put_time(ptm, "%c") << "] ";
    return ss.str();
}


void global::set8(uint8_t* index, uint8_t val) {
    *index = val;
}

void global::set16(uint8_t* index, uint16_t val) {
    set8(index, (val >> 8) & 0xFF);
    set8(index + 1, val & 0xFF);
}

void global::set32(uint8_t* index, uint32_t val) {
    set16(index, (val >> 16) & 0xFFFF);
    set16(index + 2, val & 0xFFFF);
}

uint8_t global::get8(uint8_t* index) {
    return *index;
}

uint16_t global::get16(uint8_t* index) {
    return (static_cast<uint16_t>(get8(index)) << 8) | get8(index + 1);
}

uint32_t global::get32(uint8_t* index) {
    return (static_cast<uint32_t>(get16(index)) << 16) | get16(index + 2);
}