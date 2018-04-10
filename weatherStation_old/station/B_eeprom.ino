/**
 * put a boolean in a byte at a specific position (0-7)
 */
byte putBool(byte in, byte pos, bool val) {
  return (val ? (in | (1 << pos)) : (in & ~(1 << pos))); 
}

/**
 * extract a boolean from a byte at a specific position (0-7)
 */
bool getBool(byte in, byte pos) {
  return (in & (1 << pos)) != 0;
}

/*
 * write an integer to EEPROM
 * 
 * Note: This uses two bytes
 */
void EEPROMwriteInt(int address, int value) {
  //Seperate the integer into two bytes.
  byte low = (byte)value;
  byte high = (byte)(value >> 8);
  EEPROM.update(address, high);
  EEPROM.update(address+1, low);
  //DEBUG_PRINT(address);DEBUG_PRINT(": ");DEBUG_PRINTLN(value);
}

/**
 * read an integer from EEPROM
 */
int EEPROMreadInt(int address) {
  //read the two bytes and combine them into one int.
  byte high = EEPROM.read(address);
  byte low = EEPROM.read(address+1);
  //DEBUG_PRINT(address);DEBUG_PRINT(": ");DEBUG_PRINTLN((((int)high) << 8) | low);
  return (((int)high) << 8) | low;
}



/**
 * write a double to EEPROM with x number of decimal places.
 * 
 * Note: that this uses 5 bytes of EEPROM to store.
 */
void EEPROMwriteDouble(int address, double value) {
  //convert the double into a long.
  
  int n = 0;
  double tmp;
  //run until the double is now a long
  while(n < 32 && (long)tmp != tmp) {
    tmp = value*pow(2, n);
    n++;
  }
  n--;
  long adjusted = (long)(value*(pow(2, n)));
  //DEBUG_PRINT("exp: ");DEBUG_PRINTLN(n);

  //split the long into two ints
  uint16_t high = (adjusted >> 16);
  uint16_t low = ((uint16_t)(adjusted));

  //save the two ints to eeprom
  EEPROMwriteInt(address, high);
  EEPROMwriteInt(address+2, low);
  //DEBUG_PRINT(address);DEBUG_PRINT(": ");DEBUG_PRINT_FLT(value, 10);DEBUG_PRINTLN();
  //save the power of the double
  EEPROM.update(address+4, n);
}

/**
 * read a double from EEPROM
 */
double EEPROMreadDouble(int address) {
  //read the two ints
  uint16_t high = EEPROMreadInt(address);
  uint16_t low = EEPROMreadInt(address+2);

  //get the power of the number
  byte places = EEPROM.read(address+4);
  //find the double
  double num = ((double)(((long)high << 16) | low))/pow(2, places);
  //DEBUG_PRINT(address);DEBUG_PRINT(": ");DEBUG_PRINT_FLT(num, 10);DEBUG_PRINTLN();
  return num;
}

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

void debugSettings() {
  DEBUG_PRINT("dirmul: ");DEBUG_PRINTLN(settings.dirmul);
  DEBUG_PRINT("fahrenheight: ");DEBUG_PRINTLN(settings.fahrenheight);
  DEBUG_PRINT("resetRainOnRead: ");DEBUG_PRINTLN(settings.resetRainOnRead);
  DEBUG_PRINT("windTime: ");DEBUG_PRINTLN(settings.windTime);
  DEBUG_PRINT("rpmtomph: ");DEBUG_PRINTLN(settings.rpmtomph);
  DEBUG_PRINT("n: ");DEBUG_PRINTLN(settings.n);
  DEBUG_PRINT("altitude: ");DEBUG_PRINTLN(settings.altitude);
  DEBUG_PRINT("pressureconversion: ");DEBUG_PRINTLN(settings.pressureconversion);
}

void persistSettings() {
  EEPROM.update(0, EEPROMVERSION);
  byte boolreg = putBool(0, 0, settings.dirmul);
  boolreg = putBool(boolreg, 1, settings.fahrenheight);
  boolreg = putBool(boolreg, 2, settings.resetRainOnRead);
  EEPROM.update(1, boolreg);
  EEPROMwriteInt(2, settings.windTime);
  EEPROMwriteDouble(4, settings.rpmtomph);
  EEPROMwriteInt(9, settings.n);
  EEPROMwriteInt(11, settings.altitude);
  EEPROMwriteDouble(13, settings.pressureconversion);
  debugSettings();
}

void retreiveSettings() {
  byte boolreg = EEPROM.read(1);
  settings.dirmul = getBool(boolreg, 0);
  settings.fahrenheight = getBool(boolreg, 1);
  settings.resetRainOnRead = getBool(boolreg, 2);
  
  settings.windTime = EEPROMreadInt(2);
  settings.rpmtomph = EEPROMreadDouble(4);
  settings.n = EEPROMreadInt(9);
  settings.altitude = EEPROMreadInt(11);
  settings.pressureconversion = EEPROMreadDouble(13);

  debugSettings();
}
