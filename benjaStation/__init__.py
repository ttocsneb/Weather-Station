import os

from blueweather.plugin import types
from blueweather.config import Config

from . import db


class BenjaStation(types.StartupPlugin,
                   types.WeatherPlugin):

    CONFIG_NAME = 'benjaStation'
    DAEMON_FOLDER = '../baseStation'

    def __init__(self):
        super(BenjaStation, self).__init__()

        self._database = None

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

    def on_after_startup(self):
        self._logger.info("Starting BenjaStation!")

        os.popen('sudo service baseStation start')

    def on_shutdown(self):
        self._logger.info("Stopping BenjaStation!")
        os.popen('sudo service baseStation stop')

    def _load_defaults(self):
        self._logger.info("No configuration setup, loading defaults")
        config = dict()

        config['user'] = 'blue'
        config['dbServer'] = 'localhost'

        self._config[self.__class__.CONFIG_NAME] = config
        self._config.save()
        self._config.load()
