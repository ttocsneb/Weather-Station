

class Packet:

    def __init__(self, size: int = None, packet=None):
        if size is not None:
            self._array = [0 for _ in range(size)]
        elif packet is not None:
            self._array = bytearray(packet)
        else:
            raise RuntimeError

    @property
    def value(self):
        return bytearray(self._array)

    def setBool(self, index, location, value):
        if value:
            self._array[index] |= (1 << location)
        else:
            self._array[index] &= ~(1 << location)

    def set8(self, index, value):
        self._array[index] = int(value)

    def set16(self, index, value):
        self.set8(index, (value >> 8) & 0xFF)
        self.set8(index + 1, value & 0xFF)

    def set32(self, index, value):
        self.set16(index, (value >> 16) & 0xFFFF)
        self.set16(index + 2, value & 0xFFFF)

    def getBool(self, index, location):
        return ((self._array[index] >> location) & 1) == 1

    def get8(self, index):
        return self._array[index]

    def get16(self, index):
        return (self.get8(index) << 8) | self.get8(index + 1)

    def get32(self, index):
        return (self.get16(index) << 16) | self.get16(index + 1)


class Command:

    def __init__(self, command: Packet = None, expectReply: bool = False):
        self.command = command
        self.expectReply = expectReply
