#include <iostream>
#include <sstream>
#include <iomanip>
#include <wiringPi.h>

#include "main.h"
#include "radio.h"
#include "eprom.h"


using std::cout;
using std::endl;

int main(int argc, char** argv) {
    wiringPiSetup();

    pinMode(27, OUTPUT);

    radio::begin();

    eeprom::loadEEPROM();

    while(true) {
        time_point t = system_clock::now() + 30s;
        radio::update();
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