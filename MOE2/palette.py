# -*- coding:utf-8 -*-

import struct

from log import Log

logger = Log.get_logger("engine")


class Palette(object):
    def __init__(self, window, id, colors):
        self._id = id
        self._window = window
        self._lut = []
        self._lut_map = {}
        self._pixel_bits = 0

        self._lut = colors

        for i, color in enumerate(self._lut):
            self._lut_map[color] = i

    @property
    def pixel_bits(self):
        pixel_bits = 1
        while (1 << pixel_bits) < len(self._lut):
            pixel_bits *= 2
        return 8 if pixel_bits <= 8 else pixel_bits

    @property
    def id(self):
        return self._id

    @property
    def count(self):
        return len(self._lut)

    def find_color(self, color):
        if color in self._lut_map:
            return self._lut_map[color]
        else:
            return None

    def add_color(self, color):
        self._lut.append(color)
        self._lut_map[color] = len(self._lut) - 1
        return len(self._lut) - 1

    def color(self, index):
        assert index < len(self._lut), "{} should < {}".format(index, len(self._lut))
        return self._lut[index]

    def extend(self, color_data, transparent_color):
        data = []
        for color in color_data:
            if color == transparent_color:
                color |= 0xFF000000
            index = self.find_color(color)
            if index is None:
                data.append(self.add_color(color))
            else:
                data.append(index)
        return data

    def __str__(self):
        return "%s(id: %s, size:%d)" % \
               (type(self), self._id, len(self._lut))

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        lut_len = len(self._lut)
        header = struct.pack('<BxH',
                             self.pixel_bits,
                             len(self._lut))
        if len(self._lut) > 0:
            ram = struct.pack('<%dI' % len(self._lut), *self._lut)
            header += struct.pack('<I', ram_offset)
        else:
            ram = b''
            header += struct.pack('<I', 0)
        return header, ram
