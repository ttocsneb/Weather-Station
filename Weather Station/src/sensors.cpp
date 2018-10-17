#include "sensors.h"

#include "queue.h"
#include "eprom.h"


DHT sensors::dht(DHTPIN, DHTTYPE);
FaBoBarometer sensors::baro;


uint16_t sensors::maxWind;
uint16_t sensors::maxDirection;
uint16_t sensors::averageWind;
uint16_t sensors::averageDirection;


volatile uint8_t sensors::rain;


QueueArray<uint8_t> windTick;
QueueArray<uint16_t> averageSpeed;
QueueArray<uint8_t> averageDir;


uint16_t ticks;


void sensors::begin() {
    dht.begin();
    baro.begin();

    //setup rain interrupt
    PCICR = 1;
    PCMSK0 |= 1 << digitalPinToPCMSKbit(RAIN);

    reset();

    //fill the queue with empty data
    uint8_t s = eeprom::wind::queue_size;
    while(s--) {
        DEBUG_PRINTLN(s);
        windTick.push(0);
    }

    s = eeprom::wind::average_queue_size;
    uint8_t dir = getWindDirection() / 45;
    while(s--) {
        averageSpeed.push(0);
        averageDir.push(dir);
    }
}


uint8_t sensors::getRainFall() {
    return rain;
}


int16_t sensors::getTemperature() {
    return static_cast<int16_t>(dht.readTemperature() * 10);
}


uint16_t sensors::getHumidity() {
    return static_cast<int16_t>(dht.readHumidity());
}


uint16_t sensors::getPressure() {
    return baro.readHpa(eeprom::pressure::altitude) * 10;
}



void sensors::update() {
    static bool lastWindState(false);

    static long lastWindUpdate(0);
    static long lastAverageUpdate(0);

    long current_time = millis();

    // Update the current windtick
    if(current_time - lastWindUpdate > eeprom::wind::update) {
        lastWindUpdate = current_time;
        ticks = 0;
        for(uint8_t i = 0; i < windTick.count(); i++) {
            ticks += windTick[i];
        }

        ticks /= windTick.count();

        if(getWindSpeed() > maxWind) {
            maxWind = getWindSpeed();
            maxDirection = getWindDirection();
        }

        windTick.pop();
        windTick.push(0);
    }

    // Update the current Average tick
    if(current_time - lastAverageUpdate > eeprom::wind::average_update) {
        lastAverageUpdate = current_time;
        // discard the oldest average tick, and add the newest
        averageSpeed.pop();
        averageSpeed.push(getWindSpeed());
        averageDir.pop();
        averageDir.push(getWindDirection() / 45);

        averageWind = 0;
        averageDirection = 0;
        for(uint8_t i = 0; i < averageSpeed.count(); i++) {
            averageWind += averageSpeed[i];
            averageDirection += averageDir[i];
        }
        averageWind /= averageSpeed.count();
        averageDirection = (averageDirection * 45) / averageSpeed.count();
    }


    // Wait until either the windstate changes, or the shortest timer elapses.
    // This should be the only source of delay in the program
    while(lastWindState == digitalRead(WIND) && millis() - current_time < eeprom::shortest_delay) delayMicroseconds(500);

    // If the windstate has changed, log it the the current wind_tick
    if(lastWindState != digitalRead(WIND)) {
        lastWindState = !lastWindState;
        if(lastWindState) {
            windTick[windTick.count() - 1] += 1;
        }
    }
}


uint8_t sensors::getWindSpeed() {
    return ticks;
}


#define north     554
#define northeast 226
#define east      35
#define southeast 66
#define south     122
#define southwest 366
#define west      0
#define northwest 692


uint16_t sensors::getWindDirection() {
    do {
        int dir = analogRead(VAINPIN);
        if(abs(dir - west) < 10) {
            return 270;
        }
        if(abs(dir - northwest) < 10) {
            return 315;
        }
        if(abs(dir - north) < 10) {
            return 0;
        }
        if(abs(dir - northeast) < 10) {
            return 45;
        }
        if(abs(dir - east) < 10) {
            return 90;
        }
        if(abs(dir - southeast) < 10) {
            return 135;
        }
        if(abs(dir - south) < 10) {
            return 180;
        }
        if(abs(dir - southwest) < 10) {
            return 225;
        }
        //the weather vain most likely just changed and is still
        // equalizing the resistance, try again until we get one of
        // the eight directions.
        delay(1);
    } while(true);
}


uint16_t sensors::getMaxWindSpeed() {
    return maxWind;
}


uint16_t sensors::getMaxWindDirection() {
    return maxDirection;
}


uint16_t sensors::getAverageWindSpeed() {
    return averageWind;
}


uint16_t sensors::getAverageWindDirection() {
    return averageDirection;
}


ISR(PCINT0_vect) {
    static long lastTime(0);
    static bool rainTick(false);

    if(digitalRead(RAIN) == 0 && millis() - lastTime > 300) {
        rainTick = true;
    }
    if(digitalRead(RAIN) != 0 && rainTick == true) {
        lastTime = millis();
        rainTick = false;
        DEBUG_PRINTLN("RainTick!");
        sensors::rain++;
    }
}


void sensors::loadPacket(uint8_t* packet) {
    main::set8(packet + WIND_DIREC_LOC, getWindDirection() / 45);
    main::set8(packet + WIND_MAX_DIREC_LOC, getMaxWindDirection() / 45);
    main::set16(packet + WIND_AVG_DIREC_LOC, getAverageWindDirection());
    main::set16(packet + WIND_SPEED_LOC, getWindSpeed());
    main::set16(packet + WIND_MAX_SPEED_LOC, getMaxWindSpeed());
    main::set16(packet + WIND_AVG_SPEED_LOC, getAverageWindSpeed());

    main::set8(packet + RAIN_LOC, getRainFall());

    main::set16(packet + TEMPERATURE_LOC, getTemperature());
    main::set8(packet + HUMIDITY_LOC, getHumidity());
    main::set16(packet + PRESSURE_LOC, getPressure());
}


void sensors::reset() {
    maxWind = 0;
    rain = 0;
}