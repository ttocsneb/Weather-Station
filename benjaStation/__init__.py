import os
import threading

from blueweather.plugin import types
from blueweather.config import Config


from . import db, weather


class BenjaStation(types.StartupPlugin,
                   types.WeatherPlugin):

    CONFIG_NAME = 'benjaStation'
    DAEMON_FOLDER = '../baseStation'

    MESSAGE_REPORT = __name__ + '-reporting'
    TABLE_STATUS = __name__ + '-status'
    TABLE_BASE = __name__ + '-base'
    TABLE_WEATHER = __name__ + '-weather'
    TABLE_WIND = __name__ + '-wind'

    def __init__(self):
        super(BenjaStation, self).__init__()

        self._database = None

        self._loop = weather.Weather()
        self._loop_thread = threading.Thread(target=self._loop.start)

    def on_startup(self, host, port):
        # Setup settings

        # Temporary thing to prevent pylint errors
        # self._config = Config('foo')  # TODO comment out

        self._logger.info('current file %s', __file__)

        if self.__class__.CONFIG_NAME not in self._config:
            self._load_defaults()

        config = self._config[self.__class__.CONFIG_NAME]

        # Set up the database
        self._database = db.Database(config.get('dbServer', 'localhost'),
                                     config['user'],
                                     config.get('password', ''))

        # Set Loop variables
        self._loop._logger = self._logger
        self._loop._db = self._database

    def on_after_startup(self):
        self._logger.info("Starting BenjaStation!")

        os.popen('sudo service baseStation start')

        self._loop_thread.start()

    def on_shutdown(self):
        self._logger.info("Stopping BenjaStation!")

        os.popen('sudo service baseStation stop')
        self._loop.active = False

    def _load_defaults(self):
        self._logger.info("No configuration setup, loading defaults")
        config = dict()

        config['user'] = 'blue'
        config['dbServer'] = 'localhost'

        self._config[self.__class__.CONFIG_NAME] = config
        self._config.save()
        self._config.load()

    def on_weather_request(self):
        self._weather.table[self.__class__.TABLE_WEATHER] = self._loop.weather
        self._weather.table[self.__class__.TABLE_WIND] = self._loop.wind

    def on_status_request(self):
        self._status.table[self.__class__.TABLE_STATUS] = self._loop.status
        self._status.table[self.__class__.TABLE_BASE] = self._loop.baseStatus

        messages = ('Station is reporting', 'Station is not reporting')
        categories = ('success', 'danger')
        report = 0 if self._loop.reporting else 1
        self._status.setStatusMessage(self.__class__.MESSAGE_REPORT,
                                      messages[report],
                                      categories[report],
                                      closeable=False)
