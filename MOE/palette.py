# -*- coding:utf-8 -*-

import struct

from enumerate import *
from log import Log
from osdobject import OSDObject

logger = Log.get_logger("engine")


class Palette(OSDObject):
    def __init__(self, scene, id, colors):
        self._id = id
        self._scene = scene
        self._lut = []
        self._lut_map = {}
        self._pixel_bits = 0
        if isinstance(colors, str):
            if colors == 'RGB24':
                self._pixel_format = PixelFormat.RGB
            elif colors == 'GRAYSCALE':
                self._pixel_format = PixelFormat.GRAY_SCALE
            else:
                raise Exception('Unknown colors %s' % colors)
        else:
            self._pixel_format = PixelFormat.LUT
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
    def pixel_format(self):
        return self._pixel_format

    @property
    def id(self):
        return self._id

    def get_color_index(self, color):
        if color in self._lut_map:
            return self._lut_map[color]
        else:
            self._lut.append(color)
            self._lut_map[color] = len(self._lut) - 1
            return len(self._lut) - 1

    def color(self, index, based_color=None):
        if self._pixel_format == PixelFormat.RGB:
            return index
        else:
            assert index < len(self._lut), "{} should < {}".format(index, len(self._lut))
            return self._lut[index]

    def __str__(self):
        return "%s(id: %s, %s, size:%d)" % \
               (type(self), self._id, self._pixel_format, len(self._lut))

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        lut_len = len(self._lut)
        bins = struct.pack('<BBH',
                           self._pixel_format.value,
                           self.pixel_bits,
                           len(self._lut))
        if len(self._lut) > 0:
            ram = struct.pack('<%dI' % len(self._lut), *self._lut)
            bins += struct.pack('<I', ram_offset)
        else:
            ram = b''
            bins += struct.pack('<I', 0)
        return bins, ram
