#ifndef SQL_H
#define SQL_H

#include <string>
#include <stdint.h>

//MySQL Connector/C++ Documentation: https://dev.mysql.com/doc/dev/connector-cpp/8.0/

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
     * Commit the changes to the SQL database
     * 
     * This should be called last after all other actions 
     * have been made, as the connection is deleted after
     * the changes are commited.
     * 
     * @return true if successful
     */
    bool commit();

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