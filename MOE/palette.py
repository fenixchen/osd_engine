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

    @property
    def pixel_bits(self):
        pixel_bits = 1
        while (1 << pixel_bits) < len(self._lut):
            pixel_bits *= 2
        return pixel_bits

    @property
    def id(self):
        return self._id

    def get_color_index(self, color):
        index = 0
        for c in self._lut:
            if c == color:
                return index
            index += 1
        self._lut.append(color)
        return index

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
        logger.debug('Generate palette <%s>' % self._id)
        lut_len = len(self._lut)
        bins = struct.pack('<BBH',
                           self._pixel_format.value,
                           self.pixel_bits,
                           len(self._lut))
        if len(self._lut) > 0:
            ram = struct.pack('<%sI' % len(self._lut), *self._lut)
            bins += struct.pack('<I', ram_offset)
        else:
            ram = b''
            bins += struct.pack('<I', 0)
        return bins, ram
