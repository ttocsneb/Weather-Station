#include "eprom.h"
#include "main.h"

#include <string>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

#define getName(x) #x

#define EEPROM_FILE ".eeprom"

uint32_t eeprom::refreshTime = 30000;
uint32_t eeprom::listenTime = 2000;
float eeprom::windConversion = 1.0;
unsigned int eeprom::dailyRain = 0;


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
    cout << date() << "> Loading EEPROM" << endl;

    std::ifstream in(EEPROM_FILE);

    if(!in.is_open()) {
        cout << "Cannot open EEPROM File" << endl;
        cout << "Creating EEPROM File with current values" << endl;
        setEEPROM();
        return;
    }

    while(in.good()) {
        std::string line;
        in >> line;

        if(get_value(in, line, value(refreshTime)))
            continue;
        if(get_value(in, line, value(listenTime)))
            continue;
        if(get_value(in, line, value(windConversion)))
            continue;
        if(get_value(in, line, value(dailyRain)))
            continue;
        
    }

    cout << date() << "< EEPROM Loaded" << endl;
}

void eeprom::setEEPROM() {
    cout << date() << "> Setting EEPROM" << endl;

    std::ofstream out(EEPROM_FILE);

    if(!out.is_open()) {
        cout << "Cannot open EEPROM File" << endl;
        cout << "Check if '" << EEPROM_FILE << "' exists as a folder, or is writeprotected" << endl;
        return;
    }

    set_value(out, value(refreshTime));
    set_value(out, value(listenTime));
    set_value(out, value(windConversion));
    set_value(out, value(dailyRain));

    cout << date() << "< EEPROM Set" << endl;
}

void eeprom::syncEEPROM() {
    cout << date() << "eeprom::syncEEPROM() not implemented" << endl;
}
