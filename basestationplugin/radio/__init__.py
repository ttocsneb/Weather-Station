import os
import logging
import time

from RF24 import RF24, rf24_datarate_e
from RPi import GPIO

from ..settings import settings

from .packet import Packet
from . import commands


class Radio:

    station_address = b"WTRst"
    base_address = b"WTRbs"

    debug_led_pin = 27

    packet_size = 20

    __radio = RF24(25, 0)

    def __init__(self, logger_name=__name__, level=logging.INFO):
        # Initialize the class objects
        self._commands = commands.Commands()
        self._connected = False

        self._active = False
        self._lost_packets = 100

        self._logger = logging.getLogger(logger_name)
        self._logger.setLevel(level)

        self._logger.info("setting up radio")

        # Setup debug led
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(self.__class__.debug_led_pin, GPIO.OUT)

        # Setup Radio
        radio = self.__class__.__radio

        radio.begin()

        radio.setDataRate(rf24_datarate_e.RF24_250KBPS)
        radio.setRetries(15, 15)
        radio.payloadSize = self.__class__.packet_size

        radio.openWritingPipe(self.__class__.station_address)
        radio.openReadingPipe(1, self.__class__.base_address)

        self.deactivate()

        radio.printDetails()

        # Allow Some time for the GPIO to be set
        time.sleep(0.1)

        self._logger.info("Setup Radio")

    @property
    def commands(self):
        return self._commands

    def _checkForPacket(self):
        """
        Check if there is any available packet

        Process any available packet.
        """
        if self.active:
            available, _ = self.__class__.__radio.available()
            if available:
                self._logger.debug("Packet Received")
                data = self.__class__.__radio.read()
                packet = Packet(packet=data)
                self._commands.getReply(packet)

                self._connected = True

                return True
        return False

    def _sendPackets(self):
        """
        Send all the packets in the Queue

        :return bool: True if a response is expected from the station
        """
        expectResponse = False

        while self._commands.moreCommands():
            command = self._commands.popCommand()
            packet = command.command
            expectResp = command.expectReply

            if expectResp:
                expectResponse = True

            if not self.__class__.__radio.write(packet.value):
                self._logger.error("Could not Send Command!")
                self._connected = False
                return expectResponse

        return expectResponse

    def _sendCommands(self):
        """
        Send All the commands in the queue

        If there are any expected replies, wait for them.
        """
        self.__class__.__radio.stopListening()

        if self._sendPackets():
            self._logger.info("Expecting Reply")
            self.__class__.__radio.startListening()
            time.sleep(0.5)
            if not self.__class__.__radio.available()[0]:
                self._logger.error("No Reply has come!")
                self._connected = False
            while self.__class__.__radio.available()[0]:
                data = self.__class__.__radio.read()
                packet = Packet(packet=data)
                self._commands.getReply(packet)

    def update_weather(self):
        """
        Update the Weather.

        Should be called once every `settings.refresh_time`

        In the event of a timing drift, a number of seconds will be given to
        adjust the time between calls

        Example:
        ```
        success, time_adjust = radio.update_weather()

        time.sleep(30 + time_adjust)
        ```

        :return bool: whether the weather was successfully retreived
        :return int: number of seconds to adjust the timing by
        """
        successful = False
        reloadTime = 0

        self._logger.info("Waiting For transmissions..")

        self.__activate()
        self.__class__.__radio.startListening()

        # ============ Recover from A sync Error  ============ 
        if self._lost_packets > 1:
            self._logger.info("Out of Sync, Resyncing..")

            self._lost_packets = 0

            # Wait an entire Refresh time for the packet.
            count = 0
            for count in range(settings.refresh_time / settings.listen_time):
                successful = self._checkForPacket()
                if successful:
                    break
                time.sleep(settings.listen_time)

            if successful:
                self._sendCommands()
            else:
                self._logger.error(
                    "Could Not Resync! Is the radio on or in range?")
                self._lost_packets += 1

            self.__class__.__radio.stopListening()
            self.deactivate()

            if successful:
                self._logger.info("Received Packet, Syncing..")

                return True, \
                    count * settings.listen_time - settings.refresh_time / 2

            self._connected = False
            return False, 0
        else:  # ============ Standard Operation ============ 

            # Wait for a packet
            count = 0
            for count in range(4):
                successful = self._checkForPacket()
                if successful:
                    break
                time.sleep(settings.listen_time)

            # Determine the offset time
            count -= (4 / 2)
            if count is not 0:
                reloadTime = count * settings.listen_time
                self._logger.info("Desynced by {}s".format(reloadTime))

            # Send any commands if the
            if successful:
                self._lost_packets = min(self._lost_packets - 1, 0)
                self._sendCommands()
            else:
                self._lost_packets += 1
                self._connected = False

        self.__class__.__radio.stopListening()
        self.deactivate()

        return successful, reloadTime

    @property
    def active(self):
        return self._active

    def __activate(self):
        """
        Activate the Radio and turn on the debug led
        """
        self._active = True
        GPIO.output(self.__class__.debug_led_pin, GPIO.HIGH)
        self.__class__.__radio.powerUp()

    def deactivate(self):
        """
        Deactivate the Radio and turn off the debug led
        """
        self._active = False
        GPIO.output(self.__class__.debug_led_pin, GPIO.LOW)
        self.__class__.__radio.powerDown()
