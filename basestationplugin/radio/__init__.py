import logging
import time

from RF24 import RF24, rf24_datarate_e
from RPi import GPIO

from basestationplugin.settings import settings

from .packet import Packet
from . import commands


class Radio:

    station_address = b"WTRst"
    base_address = b"WTRbs"

    debug_led_pin = 27

    packet_size = 20

    __radio = RF24(25, 0)

    def __init__(self, level=logging.INFO):
        self._commands = commands.Commands()

        self._active = False
        self._lost_packets = 100

        self._logger = logging.getLogger(__name__)
        self._logger.setLevel(level)

        # Setup debug led
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.__class__.debug_led_pin, GPIO.OUT)

        # Setup Radio
        radio = self.__class__.__radio

        radio.begin()

        radio.setDataRate(rf24_datarate_e.RF24_250KBPS)
        radio.setRetries(15, 15)
        radio.setPayloadSize(self.__class__.packet_size)

        radio.openWritingPipe(self.__class__.station_address)
        radio.openReadingPipe(1, self.__class__.base_address)

        self.__deactivate()

        radio.printDetails()

        self._logger.log("Setup Radio")

    @property
    def commands(self):
        return self._commands

    def _checkForPacket(self):
        if self.active:
            available, _ = self.__class__.__radio.available()
            if available:
                self._logger.debug("Packet Received")
                data = self.__class__.__radio.read()
                packet = Packet(packet=data)
                self._commands.getReply(packet)

                return True
        return False

    def _sendPackets(self):
        expectResponse = False

        while self._commands.moreCommands():
            command = self._commands.popCommand()
            packet = command.command
            expectResp = command.expectReply

            if expectResp:
                expectResponse = True

            if not self.__class__.__radio.write(packet):
                self._logger.error("Could not Send Command!")
                return expectResponse

        return expectResponse

    def _sendCommands(self):
        self.__deactivate()

        if self._sendPackets():
            self._logger.log("Expecting Reply")
            self.__activate()
            time.sleep(0.5)
            if not self.__class__.__radio.available()[0]:
                self._logger.error("No Reply has come!")
            while self.__class__.__radio.available()[0]:
                data = self.__class__.__radio.read()
                packet = Packet(packet=data)
                self._commands.getReply(packet)

    def update(self):
        successful = False
        reloadTime = 0

        reloadTime = 0

        self.__activate()
        self.__class__.__radio.startListening()

        if self._lost_packets > 1:
            self._logger.log("Out of Sync, Resyncing..")

            self._lost_packets = 0

            count = 0
            for count in range(settings.refresh_time / settings.listen_time):
                successful = self._checkForPacket()
                if successful:
                    break
                time.sleep(settings.listen_time)

            if successful:
                self._sendCommands()
            else:
                self._lost_packets += 1

            self.__class__.__radio.stopListening()
            self.__deactivate()

            if successful:
                self._logger.log("Received Packet, Syncing..")

                return True, \
                    count * settings.listen_time - settings.refresh_time / 2

            return False, 0
        else:

            count = 0
            for count in range(4):
                successful = self._checkForPacket()
                if successful:
                    break
                time.sleep(settings.listen_time)

            count -= (4 / 2)
            if count is not 0:
                reloadTime = count * settings.listen_time
                self._logger.log("Desynced by {}s".format(reloadTime))

            if successful:
                self._lost_packets = min(self._lost_packets - 1, 0)
                self._sendCommands()
            else:
                self._lost_packets += 1

        self.__class__.__radio.stopListening()
        self.__deactivate()

        return successful, reloadTime

    @property
    def active(self):
        return self._active

    def __activate(self):
        self._active = True
        GPIO.output(self.__class__.debug_led_pin, GPIO.HIGH)
        self.__class__.__radio.powerUp()

    def __deactivate(self):
        self._active = False
        GPIO.output(self.__class__.debug_led_pin, GPIO.LOW)
        self.__class__.__radio.powerDown()
