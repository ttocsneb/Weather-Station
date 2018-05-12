#include "sql.h"

#include <sstream>
#include <ctime>
#include <iomanip>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "weather.h"
#include "main.h"
#include "eprom.h"
#include "commands.h"

using std::cout;
using std::cerr;
using std::endl;

sql::Connection* connect = NULL;

void sendStatement(std::string command) {
    cout << "Sent SQL Command:" << endl << command << endl;
}

bool execute(sql::Statement* stmt, std::string command) {
    bool ret = stmt->execute(command);
    sendStatement(command);
    return ret;
}

sql::ResultSet* executeQuery(sql::Statement* stmt, std::string command) {
    sql::ResultSet* ret = stmt->executeQuery(command);
    sendStatement(command);
    return ret;
}

int executefoo(sql::Statement* stmt, std::string command) {
    int ret = stmt->executeUpdate(command);
    sendStatement(command);
    return ret;
}

void printSQLError(sql::SQLException &e) {
    cerr << "SQL Error: " << e.what();
    cerr << " (MySQL error code: " << e.getErrorCode();
    cerr << ", SQLState: " << e.getSQLState() << ")" << endl;
}

bool connectSQL() {
    if(!connect) {
        try {
            sql::Driver* driver = get_driver_instance();
            connect = driver->connect("localhost", "cpp", "");
            connect->setSchema("weather");
        } catch (sql::SQLException &e) {
            printSQLError(e);
            return false;
        }
    }
    return true;
}

bool mysql::commit() {
    if(connect) {
        try {
            connect->commit();

            delete connect;
            connect = NULL;

            return true;

        } catch(sql::SQLException &e) {
            printSQLError(e);
        }
    }
    return false;
}



bool mysql::addWeatherData() {
    if(!connectSQL()) {
        return false;
    }

    cout << "Adding WeatherData to SQL" << endl;

    //Generate the SQL command
    std::stringstream ss;
    ss << "INSERT INTO data ";
    ss << "(humidity, dewpoint, temperature, rain_hour, rain_day, ";
    ss << "pressure, wind_speed, wind_dir, wind_gust, wind_gust_dir, ";
    ss << "wind_avg, wind_avg_dir) ";
    ss << "VALUES (";
    ss << weather::humidity << ", ";
    ss << weather::dewpoint << ", ";
    ss << weather::temperature << ", ";
    ss << weather::rainHour << ", ";
    ss << weather::rainDay << ", ";
    ss << weather::barometer << ", ";
    ss << weather::windSpeed << ", ";
    ss << weather::windDirection << ", ";
    ss << weather::windGust10Min << ", ";
    ss << weather::windGustDirection10Min << ", ";
    ss << weather::averageWindSpeed2Min << ", ";
    ss << weather::averageWindDirection2Min << ")";
    std::string com = ss.str();

    try {
        sql::Statement *stmt;

        //execute the SQL command
        stmt = connect->createStatement();
        execute(stmt, com);

        delete stmt;

    } catch(sql::SQLException &e) {
        printSQLError(e);

        return false;
    }


    return true;
}

bool mysql::pruneWeatherData() {
    if(!connectSQL()) {
        return false;
    }
    
    cout << "Pruning SQL Weather Data" << endl;

    //delete all entries from data where the date 
    //is older than eeprom::sql::weatherData_storageTime hours

    std::stringstream ss;
    ss << "DELETE FROM graphdata ";
    ss << "WHERE date <= '";
    ss << global::getsqlDate(system_clock::now() 
        - std::chrono::hours(eeprom::sql::weatherData_storageTime));
    ss << "'";

    
    try {
        sql::Statement *statement = connect->createStatement();

        execute(statement, ss.str());


        delete statement;

        return true;
    } catch(sql::SQLException &e) {
        printSQLError(e);
    }
    return false;
}

bool mysql::minifyWeatherData(unsigned int age) {
    if(!connectSQL()) {
        return false;
    }

    //Get the date of the oldest entry
    const std::string GET_AGE = 
        "SELECT "
            "UNIX_TIMESTAMP(MIN(date)) AS date "
        "FROM data";

    //Insert into graphdata the average of all the rows in data
    const std::string ADD_AVERAGE = 
        "INSERT INTO graphdata "
        "SELECT "
            "FROM_UNIXTIME("
                "(UNIX_TIMESTAMP(MIN(date)) + "
                "UNIX_TIMESTAMP(MAX(date)))"
                " / 2)"
            " AS average_date,"
            " AVG(humidity),"
            " AVG(temperature),"
            " MAX(rain_hour),"
            " MAX(rain_day),"
            " AVG(pressure),"
            " AVG(wind_speed),"
            " AVG(wind_dir),"
            " MAX(wind_gust),"
            " AVG(wind_gust_dir),"
            " AVG(wind_avg),"
            " AVG(wind_avg_dir),"
            " AVG(dewpoint)"
        " FROM data";
    
    //Delete all of the rows in data except for the newest one
    const std::string REMOVE_ROWS = 
        "DELETE d FROM data AS d "
            "JOIN "
                "(SELECT MAX(date) AS date FROM data) "
                "AS lim "
            "ON d.date < lim.date";
    

    try {
        sql::Statement* stmt = connect->createStatement();

        sql::ResultSet* res = executeQuery(stmt, GET_AGE);

        //Only Minify if the oldest row is more than age minutes old
        res->first();
        uint32_t timestamp = res->getInt("date");
        std::time_t t = std::time(nullptr);
        if(t - timestamp < 60 * age) {
            return true;
        }

        cout << "Minifying Weather Data: " << endl;
        
        execute(stmt, ADD_AVERAGE);
        execute(stmt, REMOVE_ROWS);

        delete res;
        delete stmt;

        return true;
    } catch(sql::SQLException &e) {
        printSQLError(e);
    }

    return false;
}

bool mysql::updateStatus() {
    if(!connectSQL()) {
        return false;
    }

    cout << "Updating SQL Status" << endl;

    const std::string SET_STATUS = 
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
                "base_resets=?";

    try {

        sql::PreparedStatement* stmt = connect->prepareStatement(SET_STATUS);

        stmt->setDouble(1, commands::status::batteryVoltage);
        stmt->setBoolean(2, commands::status::isCharging);
        stmt->setUInt(3, commands::status::chargingTime);
        stmt->setUInt(4, commands::status::uptime);
        stmt->setUInt(5, commands::status::numResets);
        stmt->setUInt(6, commands::status::lostPackets);
        stmt->setBoolean(7, commands::status::isReporting);
        stmt->setUInt(8, commands::status::base::uptime);
        stmt->setUInt(9, commands::status::base::resets);

        stmt->execute();
        sendStatement(SET_STATUS);

        
        delete stmt;

        return true;
    } catch(sql::SQLException &e) {
        printSQLError(e);
    }

    return false;
}


bool mysql::getCommands(std::string &commands) {
    commands = "";
    if(!connectSQL()) {
        return false;
    }

    cout << "Collecting Commands from SQL" << endl;

    try {
        sql::Statement *statement = connect->createStatement();
        sql::ResultSet *result;

        //Get the commands
        result = executeQuery(statement, "SELECT * FROM commands");
        if(!result->next()) {
            cout << "Found no commands" << endl;
            return false;
        }

        cout << "Got Commands:" << endl;

        //Read the commands into the commands variable
        do {
            commands = commands + result->getString("command") + "\n";
            cout << "#" << result->getInt("id") << ": " << result->getString("command") << endl;
        } while(result->next());

        //remove the read commands
        execute(statement, "DELETE FROM commands");

        delete statement;
        delete result;

        return true;
    } catch(sql::SQLException &e) {
        printSQLError(e);
    }
    
    return false;
}