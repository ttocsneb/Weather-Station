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

uint16_t rpm;

void sensors::begin() {
    dht.begin();
    baro.begin();

    //setup rain interrupt
    PCICR = 1;
    PCMSK0 |= 1 << digitalPinToPCMSKbit(RAIN);

    reset();

    //fill the queue with empty data
    uint8_t s = eeprom::wind::ticks;
    while(s--) {
        DEBUG_PRINTLN(s);
        windTick.push(0);
    }

    s = eeprom::wind::averageTimeN;
    uint8_t dir = getWindDirection() / 45;
    while(s--) {
        averageSpeed.push(0);
        averageDir.push(dir);
    }

}

int8_t sensors::getRainFall() {
    return rain;
}

int16_t sensors::getTemperature() {
    return static_cast<int16_t>(dht.readTemperature() * 10);
}

uint16_t sensors::getHumidity() {
    return static_cast<int16_t>(dht.readHumidity() * 10);
}

uint16_t sensors::getPressure() {
    return baro.readHpa(eeprom::pressure::altitude) * 10;
}

void sensors::update() {
    static bool lastWindState(false);
    static long lastWindTime(0);

    static long lastAverageUpdate(0);

    //calculate rpm
    if(millis() - lastWindTime > eeprom::wind::tickDelay) {
        lastWindTime = millis();
        rpm = 0;
        for(uint8_t i = 0; i < windTick.count(); i++) {
            rpm += windTick[i];
        }
        rpm *= eeprom::wind::tickMultiplier;

        if(getWindSpeed() > maxWind) {
            maxWind = getWindSpeed();
            maxDirection = getWindDirection();
        }

        windTick.pop();
        windTick.push(0);
    }

    if(lastWindState != digitalRead(WIND)) {
        lastWindState = !lastWindState;
        if(lastWindState) {
            windTick[windTick.count() - 1] = windTick[windTick.count() - 1] + 1;
        }
    }

    if(millis() - lastAverageUpdate > eeprom::wind::averageUpdateTime) {
        lastAverageUpdate = millis();
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


}

uint16_t sensors::getWindSpeed() {
    return rpm;
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
        DEBUG_PRINTLN(millis() - lastTime);
        lastTime = millis();
        rainTick = false;
        DEBUG_PRINTLN("RainTick!");
        sensors::rain++;
    }
}

void set8(uint8_t* index, uint8_t val) {
    *index = val;
}

void set16(uint8_t* index, uint16_t val) {
    set8(index, (val >> 8) & 0xFF);
    set8(index + 1, val & 0xFF);
}

void sensors::loadPacket(uint8_t* packet) {
    set16(packet + WIND_SPEED_LOC, getWindSpeed());
    set16(packet + WIND_DIREC_LOC, getWindDirection());
    set16(packet + WIND_MAX_SPEED_LOC, getMaxWindSpeed());
    set16(packet + WIND_MAX_DIREC_LOC, getMaxWindDirection());
    set16(packet + WIND_AVG_SPEED_LOC, getAverageWindSpeed());
    set16(packet + WIND_AVG_DIREC_LOC, getAverageWindDirection());

    set16(packet + HUMIDITY_LOC, getHumidity());
    set16(packet + TEMPERATURE_LOC, getTemperature());
    set8(packet + RAIN_LOC, getRainFall());
    set16(packet + PRESSURE_LOC, getPressure());
}

void sensors::reset() {
    maxWind = 0;
    rain = 0;
}