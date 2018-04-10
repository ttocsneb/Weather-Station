/* setting            type    reg   bit
 * dirmul             bool    1     0
 * fahrenheight       bool    1     1
 * resetRainOnRead    bool    1     2
 * windTime           int     2
 * rpmtomph           double  4
 * n                  int     9
 * altitude           int     11
 * pressureconversion double  13
 */

#define WEATHERDUMP 87  //== 'W'
#define SETTINGDUMP 99  //== 'c'
#define STATUSDUMP  83  //== 'S'
#define PERSIST     112 //== 'p'
#define RESET       114 //== 'r'
#define RESETRAIN   108 //== 'l'

const byte thisaddr[6] = "wethr";
const byte homeaddr[6] = "home0";

#define statusSize 5
struct StatusPacket {
  byte battery;
  long runningTime;
  
};

#define weatherSize 19
uint8_t weatherdata[weatherSize];
struct WeatherPacket {
  double windspeed;
  int16_t winddir;
  double pressure;
  uint8_t humidity;
  double temperature;
  uint8_t rain;
};

void putFloat(double number, uint8_t* ar, uint8_t point) {
  uint8_t n = 0;
  double tmp;
  //run until the double is now a long
  while(n < 32 && (long)tmp != tmp) {
    tmp = number*pow(2, n);
    n++;
  }
  n--;
  long adjusted = (long)(number*(pow(2, n)));
  DEBUG_PRINT("exp: ");DEBUG_PRINTLN(n);

  //split the long into two ints
  uint16_t high = (adjusted >> 16);
  uint16_t low = ((uint16_t)(adjusted));

  ar[point] = (uint8_t)(adjusted >> 24);
  ar[point+1] = (uint8_t)(adjusted >> 16);
  ar[point+2] = (uint8_t)(adjusted >> 8);
  ar[point+3] = (uint8_t)(adjusted);
  ar[point+4] = n;
}

double getFloat(uint8_t* ar, uint8_t point) {
  long number = ((long)ar[point] << 24 )|((long)ar[point+1] << 16)|((long)ar[point+2] << 8)|(long)ar[point+3];
  return ((double)number)/pow(2, ar[point+4]);
}

RF24 radio(6, 10);

void beginRadio(){
  radio.begin();
  radio.enableDynamicPayloads();
  radio.setDataRate(RF24_250KBPS);
  //radio.setPALevel(RF24_PA_LOW);//temporary
  radio.setRetries(15, 15);

  radio.openWritingPipe(homeaddr);
  radio.openReadingPipe(1, thisaddr);

  radio.startListening();
}

bool checkforUpdates() {
  //check to see if there is anything to read, if not, do nothing.
  if(radio.available()) {
    if(radio.getDynamicPayloadSize() < 1) {
      //Corrupt payload has been flushed.
      return false;
    }
    //Get the command
    byte command = 0;
    radio.read(&command, sizeof(command));

    PORTC |= light;
    delay(25);
    PORTC &= ~light;

    //Process the commands
    
    if(command == WEATHERDUMP) {
      DEBUG_PRINTLN("====\nNew WeatherDump Request");

      //Collect weatherdata
      radio.stopListening();
      WeatherPacket packet;
      packet.windspeed = getWindSpeed();
      packet.winddir = getWindDirection();
      packet.pressure = getPressure();
      packet.humidity = getHumidity();
      packet.temperature = getTemperature();
      packet.rain = getRainfall();
      if(settings.resetRainOnRead == true) {
        resetrain();
      }

      //convert the weatherdata into a packet
      putFloat(packet.windspeed, weatherdata, 0);
      int f = getWindDirection();
      weatherdata[5] = packet.winddir >> 8;
      weatherdata[6] = packet.winddir;
      putFloat(packet.pressure, weatherdata, 7);
      weatherdata[12] = packet.humidity;
      putFloat(packet.temperature, weatherdata, 13);
      weatherdata[18] = packet.rain;

      
      DEBUG_PRINT("MPH: ");DEBUG_PRINT_FLT(packet.windspeed, 2);DEBUG_PRINTLN();

      DEBUG_PRINT("HEAD: ");DEBUG_PRINTLN(packet.winddir);
      DEBUG_PRINT("RAIN: ");DEBUG_PRINT(packet.rain);DEBUG_PRINT("; RESET: ");DEBUG_PRINTLN(settings.resetRainOnRead);

      DEBUG_PRINT("TEMP: ");DEBUG_PRINTLN(packet.temperature);
      DEBUG_PRINT("HUMI: ");DEBUG_PRINTLN(packet.humidity);
      DEBUG_PRINT("PRES: ");DEBUG_PRINTLN(packet.pressure);

      //write the weatherdata to the basestation
      if(radio.write(&weatherdata, weatherSize)) {
        DEBUG_PRINTLN("SUCCESS!");
      } else {
        DEBUG_PRINTLN("FAIL");
      }
      //Start listening for the next command, and return true;
      radio.startListening();
      return true;
    }
    if(command == SETTINGDUMP) {
      DEBUG_PRINT("====\nNew SettingDump Command");

      //wait for the packet of setting data.
      long t = millis();
      while(millis()-t < 2000) {
        if(radio.available()) {
          //read the packet.
          uint8_t d[settingSize];
          radio.read(&d, settingSize);
          DEBUG_PRINTLN();
          for(int i=0;i<settingSize;i++) {
            DEBUG_PRINT(i);DEBUG_PRINT(": ");DEBUG_PRINTLN(d[i]);
          }

          //parse the packet into the settings struct.
          settings.dirmul = (d[0] & 0b001) != 0;
          settings.fahrenheight = (d[0] & 0b010) != 0;
          settings.resetRainOnRead = (d[0] & 0b100) != 0;
          settings.windTime = (d[1] << 8)|d[2];
          settings.rpmtomph = getFloat(d, 3);//3-7
          settings.n = (d[8] << 8)|d[9];
          settings.altitude = (d[10] << 8)|d[11];
          settings.pressureconversion = getFloat(d, 12);//12-16

          //print the settings to the console.
          debugSettings();
          
          return true;
        }
        DEBUG_PRINT("-");
        delay(250);
      }
      DEBUG_PRINTLN("\nTimeout, Unable to receive settingDump");
      return false;
    }
    if(command == PERSIST) {
      //save the settings currently loaded to disk.
      DEBUG_PRINTLN("====\nReceived Persist Command");
      persistSettings();
      return true;
    }
    if(command == RESETRAIN) {
      //reset the rain count.
      DEBUG_PRINTLN("====\nReceived ResetRain Command");
      resetrain();
      return true;
    }
    if(command == STATUSDUMP) {
      DEBUG_PRINTLN("====\nReceived StatusDump Request");
      radio.stopListening();
      delay(250);

      //Get the battery voltage in decivolts.(5V=500)
      int bat = int(((((long)analogRead(battpin))*165))/256);//(*2*330/1024=*165/256)get battery voltage in centivolts (330=3.3v)
      
      uint8_t percent= 0;

      //convert the voltage into battery percentage using this table: http://www.thedroneinfo.com/wp-content/uploads/2016/12/Lipo-Table.jpg
      if(bat <= 330) {
        percent = 0;
      } else if(bat < 350) {
        percent = map(bat, 330, 350, 0, 3);
      } else if(bat < 365) {
        percent = map(bat, 350, 365, 3, 9);
      } else if (bat < 370) {
        percent = map(bat, 365, 370, 9, 15);
      } else if (bat < 375) {
        percent = map(bat, 370, 375, 15, 22);
      } else if (bat < 380) {
        percent = map(bat, 375, 380, 22, 34);
      } else if (bat < 385) {
        percent = map(bat, 380, 385, 34, 50);
      } else if (bat < 390) {
        percent = map(bat, 385, 390, 50, 60);
      } else if (bat < 395) {
        percent = map(bat, 390, 395, 60, 69);
      } else if (bat < 400) {
        percent = map(bat, 395, 400, 69, 77);
      } else if (bat < 405) {
        percent = map(bat, 400, 405, 77, 83);
      } else if (bat < 410) {
        percent = map(bat, 405, 410, 83, 89);
      } else if (bat < 415) {
        percent = map(bat, 410, 415, 89, 96);
      } else {
        percent = map(bat, 415, 420, 96, 100);
      }
      

      StatusPacket stat;
      stat.battery = percent;
      stat.runningTime = millis();

      uint8_t d[statusSize];
      d[0] = stat.battery;

      //put the status data into a packet.
      d[1] = (uint8_t)(stat.runningTime >> 24);
      d[2] = (uint8_t)(stat.runningTime >> 16);
      d[3] = (uint8_t)(stat.runningTime >> 8);
      d[4] = (uint8_t)(stat.runningTime);

      //debug the data
      DEBUG_PRINT("battery: ");DEBUG_PRINTLN(stat.battery);
      DEBUG_PRINT("RunningTime:");DEBUG_PRINTLN(stat.runningTime);

      //send the packet to the basestation.
      radio.write(&d, statusSize);

      //start listening for the next command.
      radio.startListening();
      return true;
    }
    if(command == RESET) {
      DEBUG_PRINTLN("====\nReseting in 4 seconds");
      while(true) {
        delay(225);
        DEBUG_PRINT(".");
        PORTC |= light;
        delay(25);
        PORTC &= ~light;
      }
      return true;
    }
    
  }
  return false;
}

