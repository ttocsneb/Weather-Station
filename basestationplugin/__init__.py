import sys
import os
import threading

from blueweather.plugin import types


from . import db, weather


class BasestationPlugin(types.StartupPlugin,
                        types.WeatherPlugin):

    def __init__(self):
        super().__init__()
