#include "sql.h"

#include <sstream>
#include <ctime>
#include <iomanip>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "weather.h"
#include "main.h"
#include "eprom.h"

using std::cout;
using std::endl;

sql::Connection* connect = NULL;

void printSQLError(sql::SQLException &e) {
    cout << date() << "SQL Error: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << ")" << endl;
}

bool connectSQL() {
    if(!connect) {
        try {
            sql::Driver* driver = get_driver_instance();
            connect = driver->connect("localhost", "cpp", "");
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

    cout << date() << "Adding WeatherData to SQL" << endl;

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
        stmt->execute("USE weather");
        stmt->execute(com);

        delete stmt;

        cout << date() << "Sent SQL Command: " << endl;
        cout << com << endl;

    } catch(sql::SQLException &e) {
        printSQLError(e);

        return false;
    }

    cout << date() << "done" << endl;

    return true;
}

bool mysql::pruneWeatherData() {
    if(!connectSQL()) {
        return false;
    }
    
    cout << date() << "Pruning SQL Weather Data" << endl;

    //delete all entries from data where the date 
    //is older than eeprom::sql::weatherData_storageTime hours

    time_t tt = system_clock::to_time_t(system_clock::now() 
        - std::chrono::hours(eeprom::sql::weatherData_storageTime));

    struct std::tm * ptm = std::localtime(&tt);

    std::stringstream ss;
    ss << "DELETE FROM data ";
    ss << "WHERE date <= '";
    ss << std::put_time(ptm, "%Y-%m-%d %H:%M:%S");
    ss << "'";

    cout << date() << "Command:" << endl;
    cout << ss.str() << endl;
    
    try {
        sql::Statement *statement = connect->createStatement();

        statement->execute("USE weather");
        statement->execute(ss.str());

        cout << date() << "done" << endl;
        return true;
    } catch(sql::SQLException &e) {
        printSQLError(e);
    }
    return false;
}