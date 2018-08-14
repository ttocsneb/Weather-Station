# Weather-Station
A home-made weather station that acts as a Wunderground PWS

# About

It has several parts: both hardware, and software.  The physical station is an Arduino connected to a RF24L01 radio tranciever that sends information down to a Raspberry Pi baseStation.

From there in a C++ program, the weather data is processed and sent to a database to be used by Blueweather.

### Blueweather

Blueweather is another program that I am developing.  It is a web-app built on Python-Flask.  It integrates plugins to allow for custom hardware.  The plugin built for this weatherstation starts the C++ baseStation program, and collects data from the database.  From here, Blueweather will show the weather to the user on the app, and send weatherdata to Wunderground.

# Install

#### If you are wanting to Install Blueweather, click [here](https://github.com/ttocsneb/BlueWeather) instead.

I am sorry if you want to install this software.  It is so specific to my own hardware, that trying to make it your own will make you want to pull your hair out.

## Raspberry Pi

### BlueWeather Plugin

Since this is a BlueWeather plugin, the folder must go into `~/.blueweather/plugins`.  

### baseStation Build

After that you will need to build the c++ program.  I am not sure how to instruct someone on a quest like that, so you will be partially on your own for this one.  The makefile in `baseStation` contains the names of all the c++ libraries, and lib should have all of the sources.  You can download the sources with `git submodule init && git submodule update`.  Finally you will need to setup a service which I will also not help you with.

## Arduino

Finally is the arduino.  Located in 'Weather Station', I have used PlatformIO and VSCode to program the arduino station.  That might be your best bet, but you can probably figure out something if you are not using PlatformIO.  If you are really intent on using this, and not just to learn, you will have to figure something out with the wiring.  I built it from some scraps of an old weather station.  Your hardware will no doubt be different from mine, so you might as well build off of the communications protocol to make it work.

I highly doubt that anyone will be willing to take on this challenge, but if you are, good luck.
