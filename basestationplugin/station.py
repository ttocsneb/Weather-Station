import sys


# import the basestation extension
sys.path.append('../baseStation')
import basestation


class EEPROM_Variables:

    REFRESH_TIME = 0xA1
    LISTEN_TIME = 0x17
    ALTITUDE = 0xC0
    AVG_WIND_UPDATE = 0xD0
    AVG_WIND_STORAGE = 0xD3
    WIND_UPDATE = 0xDA
    WIND_STORAGE = 0xDD


class EEPROM:

    def __init__(self):
        self.real_data = False

        self.refresh_time = 0
        self.listen_time = 0
        self.altitude = 0
        self.avg_wind_update = 0
        self.avg_wind_storage = 0
        self.wind_update = 0
        self.wind_storage = 0

    def __setitem__(self, key, value):
        if key is EEPROM_Variables.REFRESH_TIME:
            self.refresh_time = value
        elif key is EEPROM_Variables.LISTEN_TIME:
            self.listen_time = value
        elif key is EEPROM_Variables.ALTITUDE:
            self.altitude = value
        elif key is EEPROM_Variables.AVG_WIND_UPDATE:
            self.avg_wind_update = value
        elif key is EEPROM_Variables.AVG_WIND_STORAGE:
            self.avg_wind_storage = value
        elif key is EEPROM_Variables.WIND_UPDATE:
            self.wind_update = value
        elif key is EEPROM_Variables.WIND_STORAGE:
            self.wind_storage = value


class Status:

    def __init__(self):
        self.real_data = False

        self.just_started = False
        self.charging = False
        self.battery = 0.0
        self.battery_temp = 0.0
        self.lost_packets = 0
        self.isReporting = False


class Raw_Weather:

    def __init__(self):
        self.real_data = False

        self.wind_speed = 0
        self.wind_dir = 0
        self.max_wind_speed = 0
        self.max_wind_dir = 0
        self.avg_wind_speed = 0
        self.avg_wind_dir = 0
        self.humidity = 0
        self.temperature = 0
        self.rain = 0
        self.pressure = 0


class Station:

    def __init__(self):
        self.eeprom = EEPROM()
        self.status = Status()
        self.raw_weather = Raw_Weather()

        self.__on_eeprom_received = None
        self.__on_status_received = None

    # EEPROM Functions

    def __on_EEPROM_got(self, eeprom):
        self.eeprom.refresh_time = eeprom[EEPROM_Variables.REFRESH_TIME]
        self.eeprom.listen_time = eeprom[EEPROM_Variables.LISTEN_TIME]
        self.eeprom.altitude = eeprom[EEPROM_Variables.ALTITUDE]
        self.eeprom.avg_wind_update = eeprom[EEPROM_Variables.AVG_WIND_UPDATE]
        self.eeprom.avg_wind_storage = eeprom[
            EEPROM_Variables.AVG_WIND_STORAGE]
        self.eeprom.wind_update = eeprom[EEPROM_Variables.WIND_UPDATE]
        self.eeprom.wind_storage = eeprom[EEPROM_Variables.WIND_STORAGE]

        self.eeprom.real_data = True

        if callable(self.__on_eeprom_received):
            self.__on_eeprom_received()

    def get_EEPROM(self, on_received: callable = None):
        """
        Get the EEPROM values from the station.

        The station is only updated once every 30 seconds, so the given
        function will be called when the EEPROM values are updated.

        :param callable on_received: the function that will be called when the
        EEPROM data is updated
        """
        basestation.get_EEPROM(self.__on_EEPROM_got)
        self.__on_eeprom_received = on_received

    def set_EEPROM(self, var: int, value: int):
        """
        Sets an EEPROM setting on the station.

        The command can take up to 30 seconds to update on the station.

        :param int var: variable to set, use `EEPROM` to get the command values
        :param int value: value to set on the station
        """
        basestation.set_EEPROM(var, value)
        self.eeprom[var] = value

    def reset_EEPROM(self):
        """
        Reset the EEPROM settings to their defaults

        This cannot be undone
        """
        basestation.reset_EEPROM()

    # status functions

    def __on_status_got(self, status):
        self.status.just_started = status['just_started']
        self.status.charging = status['charging']
        self.status.battery = status['battery']
        self.status.battery_temp = status['battery_temp']
        self.status.lost_packets = status['lost_packets']
        self.status.isReporting = status['isReporting']

        self.status.real_data = True

        if callable(self.__on_status_received):
            self.__on_status_received()

    def get_status(self, on_received: callable = None):
        """
        Get the current status of the station

        Since the station is only polled once every 30 seconds, the status
        cannot be got immidiately.  the given function will be called when the
        status has been retreived.

        :param callable on_received: called when the status has been received
        """
        basestation.get_status(self.__on_status_got)
        self.__on_status_received = on_received

    def reset_status(self):
        """
        reset the status counters.
        """
        basestation.reset_status()

    # weather functions

    def reset_weather(self):
        """
        Reset the weather counters.

        This includes counters such as the rain gauge
        """
        basestation.reset_weather()

    # system functions

    def begin_radio(self):
        """
        Starts the radio on the baseStation

        Should be done only once, and does not need to be done again when the
        station is restarted
        """
        basestation.begin_radio()

    def update_radio(self):
        """
        Updates the radio.  This is a blocking function, so should be done
        within a seperate thread

        :returns bool success: successfully communicated with the station
        :returns int delay: time in milliseconds to adjust from the normal
        loop time
        """
        success, delay = basestation.update_radio()

        if success:
            # Update the raw weather if the station has communicated
            raw_weather = basestation.get_raw_weather()

            self.raw_weather.wind_speed = raw_weather['windSpeed']
            self.raw_weather.wind_dir = raw_weather['windDir']
            self.raw_weather.avg_wind_speed = raw_weather['avgWindSpeed']
            self.raw_weather.avg_wind_dir = raw_weather['avgWindDir']
            self.raw_weather.max_wind_speed = raw_weather['maxWindSpeed']
            self.raw_weather.max_wind_dir = raw_weather['maxWindDir']
            self.raw_weather.humidity = raw_weather['humidity']
            self.raw_weather.temperature = raw_weather['temperature']
            self.raw_weather.rain = raw_weather['rain']
            self.raw_weather.pressure = raw_weather['pressure']

            self.raw_weather.real_data = True

        delay /= 1000.0

        return success, delay

    def restart_station(self):
        """
        restart the station.
        """
        basestation.restart()
