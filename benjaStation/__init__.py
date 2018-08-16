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
        # self._config = Config('foo')

        self._logger.info('current file %s', __file__)

        if self.__class__.CONFIG_NAME not in self._config:
            self._load_defaults()

        database_file = os.path.join(self._data_folder, 'weather.db')

        self._database = db.Database(database_file)

        # Set the starting parameters for the daemon
        params = open(os.path.join(os.path.dirname(__file__),
                                   self.__class__.DAEMON_FOLDER, '.params'),
                      'w')

        params.write('database {0}\n'.format(database_file))

    def on_after_startup(self):
        self._logger.info("Starting BenjaStation!")

        os.popen('sudo service baseStation start')

    def on_shutdown(self):
        self._logger.info("Stopping BenjaStation!")
        os.popen('sudo service baseStation stop')

    def _load_defaults(self):
        self._logger.info("No configuration setup, loading defaults")
        config = dict()

        self._config[self.__class__.CONFIG_NAME] = config
        self._config.save()
        self._config.load()
