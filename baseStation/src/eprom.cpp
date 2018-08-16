#include "eprom.h"
#include "main.h"

#include <string>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

std::string removeColons(std::string str) {
    if(str.find("::") != std::string::npos) {
        return str.replace(str.find("::"), 2, "_");
    }
    return str;
}

#define getName(x) removeColons(#x)

#define EEPROM_FILE ".eeprom"
#define PARAM_FILE ".params"

uint32_t eeprom::refreshTime = 30000;
uint32_t eeprom::listenTime = 2000;

uint16_t eeprom::resets = 0;

float eeprom::weather::windConversion = 1.0;
unsigned int eeprom::weather::dailyRain = 0;

unsigned int eeprom::sql::weatherData_storageTime = 24;

std::string eeprom::params::database = "";

#define value(x) getName(x), x

template<typename T>
bool get_value(std::istream& is, std::string name, std::string token, T& value) {
    if(name == token) {
        is >> value;
        is.ignore(1000, '\n');
        D(cout << "got " << token << ": " << value << endl);
        return true;
    }
    return false;
}

bool get_string(std::istream& is, std::string name, std::string token, std::string& value) {
    if(name == token) {
        std::getline(is, value);
        D(cout << "got " << token << ": " << value << endl);
        return true;
    }
    return false;
}


template<typename T>
void set_value(std::ostream& os, std::string token, T value) {
    os << token << " " << value << endl;
    D(cout << "set " << token << ": " << value << endl);
}

void loadParams() {
    D(cout << "Loading Params" << endl);

    std::ifstream in(PARAM_FILE);

    if(!in.is_open()) {
        D(cout << "Cannot open Param File" << endl);
        return;
    }

    std::string token;
    while(in >> token) {
        if(get_value(in, token, "database", eeprom::params::database))
            continue;
    }

    D(cout << "done" << endl);
}

void eeprom::loadEEPROM() {
    D(cout << "Loading EEPROM" << endl);

    std::ifstream in(EEPROM_FILE);

    if(!in.is_open()) {
        D(cout << "Cannot open EEPROM File" << endl);
        D(cout << "Creating EEPROM File with current values" << endl);
        setEEPROM();
        return;
    }

    std::string token;
    while(in >> token) {

        if(get_value(in, token, value(refreshTime)))
            continue;
        if(get_value(in, token, value(listenTime)))
            continue;
        if(get_value(in, token, value(weather::windConversion)))
            continue;
        if(get_value(in, token, value(weather::dailyRain)))
            continue;
        if(get_value(in, token, value(sql::weatherData_storageTime)))
            continue;
        if(get_value(in, token, value(resets)))
            continue;
        
    }

    D(cout << "done" << endl);

    loadParams();
}

void eeprom::setEEPROM() {
    D(cout << "Setting EEPROM" << endl);

    std::ofstream out(EEPROM_FILE);

    if(!out.is_open()) {
        D(cout << "Cannot open EEPROM File" << endl);
        D(cout << "Check if '" << EEPROM_FILE << "' exists as a folder, or is writeprotected" << endl);
        return;
    }

    set_value(out, value(refreshTime));
    set_value(out, value(listenTime));
    set_value(out, value(weather::windConversion));
    set_value(out, value(weather::dailyRain));
    set_value(out, value(sql::weatherData_storageTime));
    set_value(out, value(resets));

    D(cout << "done" << endl);
}

void eeprom::syncEEPROM() {
    D(cout << "eeprom::syncEEPROM() not implemented" << endl);
}
