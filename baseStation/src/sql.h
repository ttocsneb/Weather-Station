#ifndef SQL_H
#define SQL_H

#include <string>
#include <stdint.h>

// I am changing the connecter to use SQLite instead: https://github.com/SRombauts/SQLiteCpp

namespace mysql {

    /**
     * Add an entry to the weather.data SQL table
     * 
     * @return true if successful
     */
    bool addWeatherData();

    /**
     * Remove the old entries from the weather.data SQL table
     * 
     * The number of items allowed is calculated from 
     * eeprom::sql::weatherData_storageTime
     * 
     * @return true if successful
     */
    bool pruneWeatherData();

    /**
     * Minify the Weather data into groups of age.
     * 
     * The minified data is put into weather.graphdata
     * 
     * @param age minimum age of the oldest row (minutes)
     * 
     * @return true if successful
     */
    bool minifyWeatherData(unsigned int age);

    /**
     * Update the status table with the current status
     * 
     * @return true if successful
     **/
    bool updateStatus();

    /**
     * Load the commands from the SQL weather.commands table
     * into the supplied array then clear the table
     * 
     * @param commands string where the commands will be put
     * 
     * @return true if successful
     */
    bool getCommands(std::string &commands);

}

#endif