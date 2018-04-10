#define DEBUG

#include <avr/wdt.h>

#include <EEPROM.h>
#define EEPROMVERSION 7

//Include the Temperature/humidity library
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN 5

//Include the barometer library
#include <FaBoBarometer_MPL115.h>
#include <Wire.h>

//Include the nRF24 library
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>


#define light 0b10000000 //PortC7 (D13)
#define battpin A9

#define anempin 0b01000000 //PortD6 (D12)
#define vainpin A0
#define rainpin 0b10000000 //PortB7 (D11)
//Use ~(NOT) to set a pin low

#ifdef DEBUG
#define DEBUG_PRINTLN(x) Serial.println(x) 
#define DEBUG_PRINT(x) Serial.print(x) 
#define DEBUG_PRINT_FLT(x, y) Serial.print(x, y) 
#define DEBUG_BEGIN(x) Serial.begin(x) 
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINT_FLT(x, y)
#define DEBUG_BEGIN(x)
#endif

byte rain;

void setup() {
  //disable the watchdog so we don't reboot too early.
  wdt_disable();
  
  //Set the red light as an output.
  DDRC |= light;
  PORTC |= light;
  
  DEBUG_BEGIN(115200);
  #ifdef DEBUG
  delay(3000);
  #endif

  byte tmp = EEPROM.read(0);
  if(tmp != EEPROMVERSION) {
    DEBUG_PRINT("New EEPROM Version: v");DEBUG_PRINT(tmp);DEBUG_PRINT(" to v");DEBUG_PRINTLN(EEPROMVERSION);
    DEBUG_PRINTLN("Reseting EEPROM to default Values!");
    persistSettings();
  }else {
    DEBUG_PRINT("Retrieving EEPROM Settings :v");DEBUG_PRINTLN(tmp);
    retreiveSettings();
  }
  pinMode(battpin, INPUT);

  beginRadio();
  
  beginbaro();

  begindht();


  //activate the anemometer pin as an Input w/ pull-up restor
  DDRD  &= ~anempin; //Set the pin to input for D12
  PORTD |= anempin; //Activate the pull-up resistor for D12

  DDRB  &= ~rainpin; // Set the D11 pin to input
  PORTB |= rainpin; //Activate the pull-up resistor for D11
  

  //setup the interrupt for the rain gauge

  PCICR = 1;
  PCMSK0 |= rainpin;

  wdt_enable(WDTO_4S);
  PORTC &= ~light;
}





void loop() {
  
  if(checkforUpdates()) {
    PORTC |= light;
    delay(200);
    PORTC &= ~light;
  }
  delay(500);
  wdt_reset();
}

long lasttime = 0;

bool rainTick;

ISR(PCINT0_vect) {

  //when detecting rain, check for a toggle and make sure there were no previous toggles in the last 0.3 seconds.
  if((PINB & rainpin) == 0 && millis()-lasttime > 300)rainTick = true;
  if((PINB & rainpin) != 0 && rainTick == true) {
    DEBUG_PRINTLN(millis()-lasttime);
    //reset the lasttime to now
    lasttime = millis();
    rainTick = false;
    DEBUG_PRINTLN("RainTick!");
    rain++;
  }
}



