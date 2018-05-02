#include "weather.h"
#include "eprom.h"
#include "radio.h"
#include "main.h"
#include <math.h>

#include <iostream>
#include <deque>

using std::cout;
using std::endl;

float weather::windSpeed;
uint16_t weather::windDirection;
float weather::windGust10Min;
uint16_t weather::windGustDirection10Min;
float weather::averageWindSpeed2Min;
uint16_t weather::averageWindDirection2Min;

float weather::humidity;
float weather::dewpoint;
float weather::temperature;
float weather::barometer;

float weather::rainHour;
float weather::rainDay;


std::deque<uint8_t> rainHourQueue;

void weather::begin() {
    //fill the rainHourQueue with the propper amount of items
    while(!rainHourQueue.empty()) {
        rainHourQueue.pop_front();
    }
    for(uint8_t i = 0; i < 3.6E6/eeprom::refreshTime; i++) {
        rainHourQueue.push_back(0);
    }
}

void weather::update() {
    cout << date() << "> Calculating weatherData" << endl;

    //wind
    windSpeed = radio::rawWindSpeed * eeprom::weather::windConversion;
    windDirection = radio::rawWindDirection;
    windGust10Min = radio::rawMaxWindSpeed * eeprom::weather::windConversion;
    windGustDirection10Min = radio::rawMaxWindDirection;
    averageWindSpeed2Min = radio::rawAverageWindSpeed * eeprom::weather::windConversion;
    averageWindDirection2Min = radio::rawAverageWindDirection;

    //humidity
    humidity = radio::rawHumidity / 10.0;

    cout << "humidity: " << humidity << endl;

    //dewpoint: http://andrew.rsmas.miami.edu/bmcnoldy/Humidity.html
    float c = radio::rawTemperature / 10.0;
    dewpoint = 243.04 * (log(humidity/100.0) + ((17.625 * c) / (243.04 + c))) / (17.625 - log(humidity / 100) - ((17.625 * c) / (243.04 + c)));
    //convert to F
    dewpoint *= 9 / 5.0;
    dewpoint += 32;

    cout << "dewpoint: " << dewpoint << endl;

    //temperature convert c to f
    temperature = ((radio::rawTemperature / 10.0) * 9 / 5 ) + 32;

    cout << "temperature: " << temperature << endl;

    //barometer convert dpa to inhg
    barometer = radio::rawPressure * 0.0029529983071445;

    cout << "barometer: " << barometer << endl;

    

    //rain Hour

    //update the queue
    rainHourQueue.pop_front();
    rainHourQueue.push_back(radio::rawRainFall);

    //get the total amount of rain for the hour
    unsigned int sum = 0;
    for(std::deque<uint8_t>::iterator i = rainHourQueue.begin(); i < rainHourQueue.end(); i++) {
        sum += *i;
    }
    rainHour = sum / 100.0;

    cout << "rain Hour: " << rainHour << endl;

    //rain day

    static unsigned int days(0);
    unsigned int currDays = time(NULL)/(60*60*24);
    bool update = false;

    //reset dailyRain every day
    if(currDays > days) {
        days = currDays;
        eeprom::weather::dailyRain = 0;
        update = true;
    }

    if(radio::rawRainFall != 0) {
        eeprom::weather::dailyRain += radio::rawRainFall;
        update = true;
    }

    rainDay = eeprom::weather::dailyRain / 100.0;

    if(update) {
        eeprom::setEEPROM();
    }

    cout << "rain Day: " << rainDay << endl;

    cout << date() << "> Weather Calculated" << endl;
}