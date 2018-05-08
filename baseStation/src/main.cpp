#include <iostream>
#include <sstream>
#include <iomanip>
#include <wiringPi.h>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>


#include "main.h"
#include "radio.h"
#include "eprom.h"
#include "weather.h"
#include "commands.h"
#include "sql.h"


using std::cout;
using std::endl;

void gotEEProm(EEPROM_Variable var) {
    switch(var) {
    case ALIAS_WIND_TICK:
        cout << date() << "Got Wind Tick: " << commands::station::wind::ticks << endl;
        return;
    case ALIAS_WIND_READ_TIME:
        cout << date() << "Got Wind Readtime: " << commands::station::wind::readTime << endl;
        return;
    case ALIAS_WIND_AVG_UPDATE_TIME:
        cout << date() << "Got Wind average Updatetime: " << commands::station::wind::averageUpdateTime << endl;
        return;
    case ALIAS_WIND_AVG_STORAGE_TIME:
        cout << date() << "Got Wind average Storagetime: " << commands::station::wind::averageStorageTime << endl;
        return;
    case ALIAS_PRES_ALTITUDE:
        cout << date() << "Got Pressure Altitude: " << commands::station::pressure::altitude << endl;
        return;
    case ALIAS_REFRESH_TIME:
        cout << date() << "Got Refresh Time: " << commands::station::refreshTime << endl;
    }
}

void gotStatus() {
    cout << date() << "Got Status: " << endl;
    cout << "Battery: " << commands::status::batteryVoltage << endl;
    cout << "Charging: " << (commands::status::isCharging ? "Yes" : "No") << endl;
    cout << "Lost Packets: " << commands::status::lostPackets << endl;
    cout << "Resets: " << commands::status::numResets << endl;
}

/**
 * upload the weather data to wunderground
 * 
 * Also add a weather.data item to the SQL Server
 */
void uploadWeather() {
    cout << date() << "Uploading WeatherData:" << endl;

    mysql::addWeatherData();
    mysql::minifyWeatherData(5);
    mysql::pruneWeatherData();
    mysql::commit();

    //upload the data
    std::string output = global::exec("./upload");

    cout << output << std::flush;
    cout << date() << "done" << endl;
}

int main(int argc, char** argv) {
    wiringPiSetup();

    global::begin();

    eeprom::loadEEPROM();
    eeprom::setEEPROM();

    weather::begin();

    radio::begin();


    while(true) {
        time_point t = system_clock::now() + std::chrono::milliseconds(eeprom::refreshTime);
        if(radio::update(t)) {
            weather::update();


            commands::getStatus(&gotStatus);
            mysql::updateStatus();

            uploadWeather();

            
            commands::parseCommandsFile();  

        }
        
        sleep_until(t);
    }

    return 0;
}

#define LIGHT_PIN 27

void global::begin() {
    pinMode(LIGHT_PIN, OUTPUT);
}

std::string global::exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
//Prevent my editor from complaining about popen()
#ifndef _WIN32
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if(!pipe) throw std::runtime_error("popen() failed!");
    while(!feof(pipe.get())) {
        if(fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
#endif
    return result;
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

std::string date() {
    std::stringstream ss;

    time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm * ptm = std::localtime(&tt);

    ss << "[" << std::put_time(ptm, "%c") << "] ";
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