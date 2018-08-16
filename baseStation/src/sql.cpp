#include "sql.h"

#include <sstream>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <exception>

#include <SQLiteCpp/SQLiteCpp.h>

#include "weather.h"
#include "main.h"
#include "eprom.h"
#include "commands.h"

std::string DATABASE_FILE = "";

using std::cout;
using std::cerr;
using std::endl;

void printError(SQLite::Exception &e) {
    cerr << "mysql error: [" << e.getErrorCode() << "] " << e.getErrorStr() << endl;

    if(e.getErrorCode() == 14) {
        cerr << "file: " << DATABASE_FILE << endl;
    }
}

void mysql::begin() {
    DATABASE_FILE = eeprom::params::database;
}

bool mysql::addWeatherData() {
    try {
        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);

        SQLite::Transaction transaction(db);

        SQLite::Statement query(db, 
            "INSERT INTO data "
            "(humidity, dewpoint, temperature, rain_hour, rain_day, "
            "pressure, wind_speed, wind_dir, wind_gust, wind_gust_dir, "
            "wind_avg, wind_avg_dir) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        
        query.bind(1, weather::humidity);
        query.bind(2, weather::dewpoint);
        query.bind(3, weather::temperature);
        query.bind(4, weather::rainHour);
        query.bind(5, weather::rainDay);
        query.bind(6, weather::barometer);
        query.bind(7, weather::windSpeed);
        query.bind(8, weather::windDirection);
        query.bind(9, weather::windGust10Min);
        query.bind(10, weather::windGustDirection10Min);
        query.bind(11, weather::averageWindSpeed2Min);
        query.bind(12, weather::averageWindDirection2Min);

        query.exec();

        transaction.commit();

        return true;
    } catch(SQLite::Exception e) {
        cerr << "Error while trying to update weather" << endl;
        printError(e);
    }
    return false;
}

bool mysql::pruneWeatherData() {
    try {

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);

        SQLite::Transaction transaction(db);
        SQLite::Statement query(db,
            "DELETE FROM graphdata "
            "Where date <= ?");
        
        query.bind(1, global::getsqlDate(system_clock::now() - 
                   std::chrono::hours(eeprom::sql::weatherData_storageTime)));
        
        query.exec();
        transaction.commit();

        return true;

    } catch(SQLite::Exception e) {
        printError(e);
    }
    return false;
}

bool mysql::minifyWeatherData(unsigned int age) {
    try {

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);

        SQLite::Statement query(db,
            "SELECT "
                "UNIX_TIMESTAMP(MIN(date)) AS date "
            "FROM data");
        
        query.executeStep();

        // Only minify the data if the oldest row is more than `age` minutes old
        uint32_t date = query.getColumn(0);
        std::time_t t = std::time(nullptr);
        if(t - date < 60 * age) {
            return true;
        }

        SQLite::Transaction transaction(db);

        // Insert one row into graphdata that is the average of all the rows in data
        db.exec(
            "INSERT INTO graphdata "
            "SELECT "
                "FROM_UNIXTIME("
                    "UNIX_TIMESTAMP(MIN(date)) + "
                    "UNIX_TIMESTAMP(MAX(date))) "
                    "/ 2) AS average_date, "
                "AVG(humidity), "
                "AVG(temperature), "
                "MAX(rain_day), "
                "MAX(rain_hour), "
                "AVG(pressure), "
                "AVG(wind_speed), "
                "AVG(wind_dir), "
                "MAX(wind_gust), "
                "AVG(wind_gust_dir), "
                "AVG(wind_avg), "
                "AVG(wind_avg_dir), "
                "AVG(dewpoint) "
            "FROM data"
        );

        // Delete everything except the newest row
        db.exec(
            "DELETE d FROM data AS d "
                "JOIN "
                    "(SELECT MAX(date) AS date FROM data) "
                    "AS lim "
                "ON d.date < lim.date"
        );

        transaction.commit();

        return true;

    } catch(SQLite::Exception e) {
        cerr << "Error while trying to prune weatherdata" << endl;
        printError(e);
    }
    return false;
}

bool mysql::updateStatus() {
    try {

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);

        SQLite::Transaction transaction(db);

        SQLite::Statement query(db,
            "UPDATE status "
                "SET "
                    "battery=?, "
                    "is_charging=?, "
                    "time_charging=?, "
                    "uptime=?, "
                    "resets=?, "
                    "lost_packets=?, "
                    "reporting=?, "
                    "base_uptime=?, "
                    "base_resets=?"
        );

        query.bind(1, commands::status::batteryVoltage);
        query.bind(2, commands::status::isCharging);
        query.bind(3, commands::status::chargingTime);
        query.bind(4, commands::status::uptime);
        query.bind(5, commands::status::numResets);
        query.bind(6, commands::status::lostPackets);
        query.bind(7, commands::status::isReporting);
        query.bind(8, commands::status::base::uptime);
        query.bind(9, commands::status::base::resets);

        query.exec();

        transaction.commit();

        return true;

    } catch(SQLite::Exception e) {
        cerr << "Error while trying to update status" << endl;
        printError(e);
    }
    return false;
}

bool mysql::getCommands(std::string &commands) {
    commands = "";

    try {

        SQLite::Database db(DATABASE_FILE, SQLite::OPEN_READWRITE);
        

        SQLite::Statement query(db,
            "SELECT * FROM commands"
        );

        if(!query.executeStep()) {
            D(cout << "Found no commands" << endl);
            return false;
        }

        do {
            // TODO: find actual column number
            const char* command = query.getColumn(1);
            commands += command + '\n';

            D(cout << "#" << query.getColumn(0) << ": " << command << endl);
        } while(query.executeStep());

        SQLite::Transaction transaction(db);
        db.exec("DELETE FROM commands");
        transaction.commit();

        return true;

    } catch(SQLite::Exception e) {
        cerr << "Error while trying to get commands" << endl;
        printError(e);
    }
    return false;
}