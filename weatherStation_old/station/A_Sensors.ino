#define settingSize 17
struct SettingsPacket {
  bool dirmul;              //vain
  bool fahrenheight;        //temp
  bool resetRainOnRead;     //rain
  int16_t windTime;         //wind
  double rpmtomph;          //wind
  int16_t n;                //vain
  int16_t altitude;         //baro
  double pressureconversion;//baro
};
SettingsPacket settings = {true, true, true, 2000, 0.020824337, 0, 1380, 0.029529983};

//==================== ANEMOMETER ====================

float getWindSpeed() {
  //find the speed of the anemometer
  int c = 0;
  long t = millis();
  //read for maxTime ms
  while(millis()-t < settings.windTime) {
    //wait for the to switch to toggle (180deg)
    while((PIND & anempin) != 0 && millis()-t <settings.windTime) delay(1);
    while((PIND & anempin) == 0 && millis()-t <settings.windTime)delay(1);
    //if the timer went off while waiting for a toggle, break out of the loop.
    if(millis()-t >= settings.windTime) break;
    //increase the counter.
    c++;
  }
  t = millis()-t;
  float rmp = c/(t*0.000033333); //find the RMP of the anemometer (Note: that the sensor activates every 180deg and timing is in ms)
  float mph = rmp*settings.rpmtomph;
  DEBUG_PRINT("Rev: ");DEBUG_PRINT(c/2.0);DEBUG_PRINT("/");DEBUG_PRINT(t/1000.0);DEBUG_PRINTLN("s");
  DEBUG_PRINT("RPM: ");DEBUG_PRINT_FLT(rmp, 2);DEBUG_PRINTLN();
  return mph;
}

//==================== WIND VAIN ====================


//The analogue value of each direction
#define north     554
#define northeast 226
#define east      35
#define southeast 66
#define south     122
#define southwest 366
#define west      0
#define northwest 692


int getWindDirection() {
  do {
    int dir = analogRead(vainpin);
    if(abs(dir-west) < 10)return settings.n + (settings.dirmul ? 270 : -270);
    if(abs(dir-northwest) < 10)return settings.n + (settings.dirmul ? 315 : -315);
    if(abs(dir-north) < 10)return settings.n;
    if(abs(dir-northeast) < 10)return settings.n + (settings.dirmul ? 45 : -45);
    if(abs(dir-east) < 10)return settings.n + (settings.dirmul ? 90 : -90);
    if(abs(dir-southeast) < 10)return settings.n + (settings.dirmul ? 135 : -135);
    if(abs(dir-south) < 10)return settings.n + (settings.dirmul ? 180 : -180);
    if(abs(dir-southwest) < 10)return settings.n + (settings.dirmul ? 225 : -225);
    //the weather vain most likely just changed and is still equalizing the resistance, try again until we get one of the eight directions.
    delay(1);
  }
  while(true);
}

//==================== RAIN GUAGE ====================

byte getRainfall() {
  return rain;
}

void resetrain() {
  rain = 0;
}

//==================== BAROMETER ====================

FaBoBarometer barometer;

void beginbaro() {
  barometer.begin();
}

double getPressure() {
  return barometer.readHpa(settings.altitude)*settings.pressureconversion;
}

double getPressureAtSeaLevel() {
  return barometer.readHpa()*settings.pressureconversion;
}

//==================== HUMIDITY/TEMPERATURE ====================

DHT dht(DHTPIN, DHTTYPE);

void begindht() {
  dht.begin();
}

double getHumidity() {
  return dht.readHumidity();
}

double getTemperature() {
  return dht.readTemperature(settings.fahrenheight);
}

