import os

from blueweather.plugin import types


class BenjaStation(types.StartupPlugin,
                   types.WeatherPlugin):

    def __init__(self):
        super(BenjaStation, self).__init__()

    def on_after_startup(self):
        self._logger.info("Starting BenjaStation!")
        os.popen('sudo service baseStation start')

    def on_shutdown(self):
        self._logger.info("Stopping BenjaStation!")
        os.popen('sudo service baseStation stop')
