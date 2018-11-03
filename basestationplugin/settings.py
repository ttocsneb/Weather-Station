from collections import MutableMapping


class Settings(MutableMapping):

    def __init__(self):
        self._version = 0
        self._settings = None

        self._initialized = False

    @property
    def version(self):
        return self._version

    def _load_settings(self, settings_obj):
        self._settings = settings_obj

    def get_defaults(self):
        return dict(
            refresh_time=30,
            listen_time=0.5,
            wind_update=0.25,
            wind_storage=2,
            avg_wind_update=1,
            avg_wind_storage=120,
            altitude=0
        )

    def on_migrate(self, target, current):
        pass

    def on_initialized(self):
        self._initialized = True

    # Values

    @property
    def refresh_time(self):
        return self['refresh_time']

    @refresh_time.setter
    def refresh_time(self, value):
        self['refresh_time'] = value

    @property
    def listen_time(self):
        return self['listen_time']

    @listen_time.setter
    def listen_time(self, value):
        self['listen_time'] = value

    @property
    def wind_update(self):
        return self['wind_update']

    @wind_update.setter
    def wind_update(self, value):
        self['wind_update'] = value

    @property
    def wind_storage(self):
        return self['wind_storage']

    @wind_storage.setter
    def wind_storage(self, value):
        self['wind_storage'] = value

    @property
    def avg_wind_update(self):
        return self['avg_wind_update']

    @avg_wind_update.setter
    def avg_wind_update(self, value):
        self['avg_wind_update'] = value

    @property
    def avg_wind_storage(self):
        return self['avg_wind_storage']

    @avg_wind_storage.setter
    def avg_wind_storage(self, value):
        self['avg_wind_storage'] = value

    # MutableMapping

    def __setitem__(self, key, value):
        if self._initialized:
            self._settings[key] = value
        raise KeyError

    def __getitem__(self, key):
        if self._initialized:
            return self._settings[key]
        raise KeyError

    def __delitem__(self, key):
        if self._initialized:
            del self._settings[key]
        raise KeyError

    def __iter__(self):
        if self._initialized:
            return iter(self._settings)
        return iter([])

    def __contains__(self, x):
        if self._initialized:
            return x in self._settings
        return False

    def __len__(self):
        if self._initialized:
            return len(self._settings)
        return 0


settings = Settings()
