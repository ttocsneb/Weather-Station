import queue
import enum

import threading

from radio.packet import Packet, Command


class Callable_Command(threading.Thread):

    def __init__(self, command, args=None, kwargs=None):
        super().__init__()
        self._command = command
        self._args = args
        self._kwargs = kwargs

    def run(self):
        if callable(self._command):
            self._command(*self._args, **self._kwargs)


class EEPROM_Variable(enum.Enum):
    REFRESH_TIME = 0xA1
    LISTEN_TIME = 0x17
    ALTITUDE = 0xC0
    AVG_WIND_UPDATE = 0xD0
    AVG_WIND_STORAGE = 0xD3
    WIND_UPDATE = 0xDA
    WIND_STORAGE = 0xDD


class Command_Bytes(enum.Enum):
    GET_WEATHER = 0xA0
    GET_SETTINGS = 0xA5
    GET_STATUS = 0xAB

    RESET = 0xB0
    RESET_STATUS = 0xB3

    RESET_SETTINGS = 0xC0
    RESTART = 0xCA

    SET_SETTINGS = 0xD5


class RawWeather:

    def __init__(self, *args, **kwargs):
        self.windDirection = kwargs.get('windDirection', 0)
        self.maxWindDirection = kwargs.get('maxWindDirection', 0)
        self.averageWindDirection = kwargs.get('averageWindDirection', 0)

        self.windSpeed = kwargs.get('windSpeed', 0)
        self.maxWindSpeed = kwargs.get('maxWindSpeed', 0)
        self.averageWindSpeed = kwargs.get('averageWindSpeed', 0)

        self.rainFall = kwargs.get('rainFall', 0)
        self.temperature = kwargs.get('temperature', 0)
        self.humidity = kwargs.get('humidity', 0)
        self.pressure = kwargs.get('pressure', 0)


class Commands:

    def __init__(self):
        self._commands = queue.Queue()
        self._eeprom_receive = queue.Queue()
        self._status_receive = queue.Queue()
        self._rawWeather = RawWeather()

    @property
    def raw_weather(self):
        return self._rawWeather

    def addCommand(self, command: Command):
        self._commands.put(command)

    def popCommand(self):
        return self._commands.get()

    def moreCommands(self):
        return not self._commands.empty()

    def setEEPROM(self, variable: int, value):
        packet = Packet(size=6)

        packet.set8(0, Command_Bytes.SET_SETTINGS)
        packet.set8(1, variable)
        packet.set32(2, value)

        self.addCommand(Command(packet, False))

    def getEEPROM(self, on_received: callable):
        packet = Packet(packet=[Command_Bytes.GET_SETTINGS])
        self.addCommand(Command(packet, True))
        self._eeprom_receive.put(on_received)

    def getStatus(self, on_received: callable):
        packet = Packet(packet=[Command_Bytes.GET_STATUS])
        self.addCommand(Command(packet, True))
        self._status_receive.put(on_received)

    def resetWeather(self):
        self.addCommand(Command((Packet(packet=[Command_Bytes.RESET]))))

    def resetStatus(self):
        self.addCommand(Command((Packet(packet=[Command_Bytes.RESET_STATUS]))))

    def restart(self):
        self.addCommand(Command((Packet(packet=[Command_Bytes.RESTART]))))

    def _loadRawWeather(self, packet: Packet):
        self._rawWeather.windDirection = packet.get8(1)
        self._rawWeather.maxWindDirection = packet.get8(2)
        self._rawWeather.averageWindDirection = packet.get16(3)
        self._rawWeather.windSpeed = packet.get16(5)
        self._rawWeather.maxWindSpeed = packet.get16(7)
        self._rawWeather.averageWindSpeed = packet.get16(9)

        self._rawWeather.rainFall = packet.get8(11)
        self._rawWeather.temperature = packet.get16(12)
        self._rawWeather.humidity = packet.get8(14)
        self._rawWeather.pressure = packet.get16(15)

    def _readEEPROM(self, packet: Packet):
        eeprom = dict(
            refresh_time=packet.get32(1),
            listen_time=packet.get16(5),
            altitude=packet.get16(7),
            avg_wind_update=packet.get16(9),
            avg_wind_storage=packet.get32(11),
            wind_update=packet.get16(15),
            wind_storage=packet.get16(17)
        )

        on_eeprom = self._eeprom_receive.get()
        if callable(on_eeprom):
            thread = Callable_Command(on_eeprom, [eeprom])
            thread.start()

    def _read_status(self, packet: Packet):
        status = dict(
            just_started=packet.getBool(1, 0),
            charging=packet.getBool(1, 1),
            battery=(packet.get8(2) / 100.0) + 1.65,
            battery_temp=packet.get16(3) / 10.0,
            lost_packets=packet.get8(5)
        )

        on_status = self._status_receive.get()
        if callable(on_status):
            thread = Callable_Command(on_status, [status])
            thread.start()

    def getReply(self, packet: Packet):
        command = packet.get8(0)

        if command == Command_Bytes.GET_WEATHER:
            self._loadRawWeather(packet)
        elif command == Command_Bytes.GET_SETTINGS:
            self._readEEPROM(packet)
        elif command == Command_Bytes.GET_STATUS:
            self._read_status(packet)


commands = Commands()
