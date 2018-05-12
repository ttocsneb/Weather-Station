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

uint32_t eeprom::refreshTime = 30000;
uint32_t eeprom::listenTime = 2000;

uint16_t eeprom::resets = 0;

float eeprom::weather::windConversion = 1.0;
unsigned int eeprom::weather::dailyRain = 0;

unsigned int eeprom::sql::weatherData_storageTime = 24;


#define value(x) getName(x), x

template<typename T>
bool get_value(std::istream& is, std::string name, std::string token, T& value) {
    if(name == token) {
        is >> value;
        cout << "got " << token << ": " << value << endl;
        return true;
    }
    return false;
}

template<typename T>
void set_value(std::ostream& os, std::string token, T value) {
    os << token << " " << value << endl;
    cout << "set " << token << ": " << value << endl;
}

void eeprom::loadEEPROM() {
    cout << "Loading EEPROM" << endl;

    std::ifstream in(EEPROM_FILE);

    if(!in.is_open()) {
        cout << "Cannot open EEPROM File" << endl;
        cout << "Creating EEPROM File with current values" << endl;
        setEEPROM();
        return;
    }

    std::string line;
    while(in >> line) {

        if(get_value(in, line, value(refreshTime)))
            continue;
        if(get_value(in, line, value(listenTime)))
            continue;
        if(get_value(in, line, value(weather::windConversion)))
            continue;
        if(get_value(in, line, value(weather::dailyRain)))
            continue;
        if(get_value(in, line, value(sql::weatherData_storageTime)))
            continue;
        if(get_value(in, line, value(resets)))
            continue;
        
    }

    cout << "done" << endl;
}

void eeprom::setEEPROM() {
    cout << "Setting EEPROM" << endl;

    std::ofstream out(EEPROM_FILE);

    if(!out.is_open()) {
        cout << "Cannot open EEPROM File" << endl;
        cout << "Check if '" << EEPROM_FILE << "' exists as a folder, or is writeprotected" << endl;
        return;
    }

    set_value(out, value(refreshTime));
    set_value(out, value(listenTime));
    set_value(out, value(weather::windConversion));
    set_value(out, value(weather::dailyRain));
    set_value(out, value(sql::weatherData_storageTime));
    set_value(out, value(resets));

    cout << "done" << endl;
}

void eeprom::syncEEPROM() {
    cout << "eeprom::syncEEPROM() not implemented" << endl;
}
