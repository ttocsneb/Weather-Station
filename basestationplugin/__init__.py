import time
import threading

import flask

from blueweather.plugin import types


from . import station, weather
from .settings import settings


class BasestationThread(threading.Thread):

    def __init__(self, station_obj, weather_obj):
        super().__init__()
        self._station = station_obj
        self._weather = weather_obj

        self._stop_event = threading.Event()

    def stop(self):
        self._stop_event.set()

    def stopped(self):
        return self._stop_event.is_set()

    def _loop(self):
        _, delay = self._station.update_radio()
        self._weather.process()

        if self.stopped():
            time.sleep(settings.refresh_time + delay)

    def run(self):
        while self.stopped():
            self._loop()


basestation = flask.Blueprint('basestation', __name__)


class BasestationPlugin(types.StartupPlugin,
                        types.TemplatePlugin,
                        types.RoutePlugin,
                        types.SettingsPlugin):

    def __init__(self):
        super().__init__()
        self._station = station.Station()
        self._weather = weather.Weather(self._station)

        self._station_thread = BasestationThread(self._station, self._weather)

    # StartupPlugin

    def on_startup(self, host, port):
        # Initialize the radio
        self._station.begin_radio()

    def on_after_startup(self):
        self._station_thread.start()

    def on_shutdown(self):
        self._station_thread.stop()
        self._station_thread.join()

    # TemplatePlugin

    def get_template_configs(self):
        return [
            dict(type='weather', template='weather.jinja2', id='bs-wtr',
                 name="Station",
                 variables=dict()),
            dict(type='weather', template='wind.jinja2', id='bs-wnd',
                 name="Station Wind",
                 variables=dict())
            # dict(type='status', template='status.jinja2',
            #      name="Station",
            #      variables=dict()),
            # dict(type='settings', template='settings.jinja2',
            #      name="Station",
            #      variables=dict())
        ]

    def get_bluprint(self):
        return basestation

    # SettingsPlugin

    def get_settings_version(self):
        return settings.version

    def get_settings_defaults(self):
        return settings.get_defaults()

    def on_settings_migrate(self, target, current):
        settings.on_migrate(target, current)

    def on_settings_initialized(self):
        settings.on_initialized()
