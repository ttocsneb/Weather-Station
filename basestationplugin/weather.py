import math
from datetime import datetime
from collections import deque

from .station import Station

# TODO: remove temporary settings
refresh_time = 30


class Wind:

    def __init__(self):
        self.speed = 0
        self.dir = 0


class Weather:

    def __init__(self, station: Station):
        self._station = station
        self._queue_rain_hour = deque()
        self._current_day = 0

        self.current_wind = Wind()
        self.average_wind = Wind()
        self.max_wind = Wind()

        self.humidity = 0
        self.pressure = 0
        self.temperature = 0
        self.dew_point = 0

        self.rain_hour = 0
        self.rain_day = 0

    def process(self):
        """
        Process the weather data from the raw weather of the station

        This should be called once after `Station.update_radio`.
        """

        # Process Wind

        # Process Humidity
        self.humidity = self._station.raw_weather.humidity / 10.0

        # Process Temperature
        self.temperature = self._station.raw_weather.temperature / 10.0

        # Process Dewpoint
        temp_calc = ((17.625 * self.temperature) / (243.04 + self.temperature))
        self.dew_point = \
            243.04 * (math.log(self.humidity / 100.0) + temp_calc) \
            / (17.625 - math.log(self.humidity / 100.0) - temp_calc)

        # Process pressure
        # convert dpa to inhg
        self.pressure = self._station.raw_weather.pressure * 0.0029529983071445

        # Process Rain

        # Rain Hour
        while len(self._queue_rain_hour) >= 3600 / refresh_time:
            self._queue_rain_hour.popleft()

        self._queue_rain_hour.append(self._station.raw_weather.rain)

        # Rain Day
        # reset the rain_day counter at midnight
        curr_hour = datetime.now().hour
        if self._current_day is not curr_hour:
            if self._current_day == 23:
                self.rain_day = 0
            self._current_day = curr_hour

        self.rain_day += self._station.raw_weather.rain

        # Wind

        self.current_wind.speed = self._station.raw_weather.wind_speed
        self.current_wind.dir = self._station.raw_weather.wind_dir * 45

        self.max_wind.speed = self._station.raw_weather.max_wind_speed
        self.max_wind.dir = self._station.raw_weather.max_wind_dir * 45

        self.average_wind.speed = self._station.raw_weather.avg_wind_speed
        self.average_wind.dir = self._station.raw_weather.avg_wind_dir

        self._station.reset_weather()
