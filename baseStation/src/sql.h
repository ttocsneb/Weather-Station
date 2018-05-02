#ifndef SQL_H
#define SQL_H

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

}

#endif