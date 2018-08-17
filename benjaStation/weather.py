import time
import datetime

from sqlalchemy import desc

from blueweather.weather import Table

from . import db


class Weather:

    def __init__(self):
        self.active = True
        self._logger = None
        self._db = None

        self.reporting = False
        self.status = Table('Station')
        self.baseStatus = Table('Base')
        self.weather = Table('Weather')
        self.wind = Table('Wind')

        # load weather table
        self.weather.width = 3
        self.weather.extend([['name', 'val'] for _ in range(7)])

        # load wind table
        self.wind.width = 3
        self.wind.extend([['name', 'value'] for _ in range(3)])

        # load status table
        self.status.width = 3
        self.status.extend([['name', 'value'] for _ in range(7)])

        # load baseStatus table
        self.baseStatus.width = 3
        self.baseStatus.extend([['name', 'value'] for _ in range(2)])

    def start(self):

        self.active = True
        while self.active:
            self.loop()
            self._wait(30)
        self._logger.info("Stopped Weather Loop")

    def _wait(self, t, step=5):
        """
        Wait for ``t`` seconds.  If the stop command is sent, then wait will
        break out of the sleep
        """
        i = 0
        while i + step < t and self.active:
            time.sleep(step)
            i += step
        if self.active:
            time.sleep(t - i)

    def loop(self):
        # Get a db session
        session = self._db.session()
        # Get the current weather data

        data = session.query(db.data).order_by(desc(db.data.date)).first()

        weather = [
            ['temperature', '{} F'.format(round(data.temperature, 1))],
            ['humidity', '{}%'.format(round(data.humidity, 1))],
            ['dewpoint', '{} F'.format(round(data.dewpoint, 1))],
            ['pressure', '{} inHg'.format(round(data.pressure, 2))],
            ['rain (hour)', '{} in'.format(round(data.rain_hour, 2))],
            ['rain (day)', '{} in'.format(round(data.rain_day, 2))],
            ['time', data.date.strftime('%a, %b %d, %Y, %I:%M %p')]
        ]

        for i, row in enumerate(weather):
            self.weather[i] = row

        wind = [
            ['current', '{} mph from the {}'.format(
                data.wind_speed,
                self._angle_to_str(data.wind_dir)
            )],
            ['gust', '{} mph from the {}'.format(
                data.wind_gust,
                self._angle_to_str(data.wind_gust_dir)
            )],
            ['average', '{} mph from the {}'.format(
                data.wind_avg,
                self._angle_to_str(data.wind_avg_dir)
            )]
        ]

        for i, row in enumerate(wind):
            self.wind[i] = row

        # update the current status

        status = session.query(db.status).first()

        status_data = [
            ['battery', '{} V'.format(status.battery)],
            ['charching', 'yes' if status.is_charging else 'no'],
            ['time spent charging', self._format_seconds(
                status.time_charging)],
            ['resets', status.resets],
            ['lost packets', status.lost_packets],
            ['uptime', self._format_seconds(status.uptime)],
            ['time', status.date.strftime('%a, %b %d, %Y, %I:%M %p')]
        ]

        for i, row in enumerate(status_data):
            self.status[i] = row

        base_status = [
            ['resets', status.base_resets],
            ['uptime', self._format_seconds(status.base_uptime)]
        ]

        for i, row in enumerate(base_status):
            self.baseStatus[i] = row

        self.reporting = status.reporting

        self._logger.info("Updated Weather")

        session.close()

    @staticmethod
    def _format_seconds(t: int) -> str:
        length = ''
        MINUTE = 60
        HOUR = 60 * MINUTE
        DAY = 24 * HOUR

        if t > DAY:
            d = round(t / DAY)
            length += str(d) + (' days, ' if d is not 1 else ' day, ')

        if t > HOUR:
            h = round(t / HOUR) % 24
            length += str(h) + (' hours, ' if h is not 1 else ' hour, ')

        if t > MINUTE:
            m = round(t / MINUTE) % 60
            length += str(m) + (' minutes, ' if m is not 1 else ' minute, ')

        s = t % 60
        length += str(s) + (' seconds' if s is not 1 else ' second')

        return length

    @staticmethod
    def _angle_to_str(angle: int) -> str:
        direction = ''
        if angle < 23 or angle > 337.5:
            direction = 'North'
        elif angle < 68:
            direction = 'North-East'
        elif angle < 113:
            direction = 'East'
        elif angle < 158:
            direction = 'South-East'
        elif angle < 203:
            direction = 'South'
        elif angle < 248:
            direction = 'South-West'
        elif angle < 293:
            direction = 'West'
        elif angle < 338:
            direction = 'North-West'
        return direction
