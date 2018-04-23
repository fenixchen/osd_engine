# -*- coding:utf-8 -*-


import struct

from enumerate import *
from font import Font
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Glyph(object):

    def __init__(self, font, char_code, font_width, font_height):
        self._data, self._width = font.load_char(char_code, font_width, font_height)
        self._font = font
        self._font_width = font_width
        self._font_height = font_height
        self._char_code = char_code
        self._object_index = None

    @property
    def object_index(self):
        assert self._object_index is not None
        return self._object_index

    @object_index.setter
    def object_index(self, object_index):
        self._object_index = object_index

    @property
    def monochrome(self):
        return self._monochrome

    @property
    def id(self):
        return '%s-%d-%d' % (self._font.id, self._font_size, self.code)

    @property
    def font(self):
        return self._font

    @property
    def code(self):
        if isinstance(self._char_code, int):
            return self._char_code
        else:
            return ord(self._char_code)

    @property
    def char_code(self):
        return self._char_code

    @property
    def font_width(self):
        return self._font_width

    @property
    def font_height(self):
        return self._font_height

    @property
    def width(self):
        return self._width

    @property
    def data(self):
        return self._data

    def bit(self, bit_offset):
        bit = self._data[bit_offset >> 3] & (128 >> (bit_offset & 7))
        return bit

    def __str__(self):
        ret = "%s(font:%s, font_width:%d, font_height:%d, char:%d, width:%d)" % (
            type(self), self._font.id, self._font_width, self._font_height, self.code,
            self._width)
        return ret

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s-%s-%d> \'%c\'' % (
            type(self), self._font.id, self.code, self._font_size, self._char_code))
        header = b''
        data = b''

        self._ram_offset = ram_offset

        header = struct.pack('<BBBB', self._left, self._top, self._width, self._height)

        header += struct.pack('<HBB', self.code, self._font.object_index, self._font_size)

        data_size = len(self._data)
        assert (data_size <= 0xFFFF)

        monochrome = 1 if self._monochrome else 0

        header += struct.pack('<BBH', self._pitch, self.advance_x,
                              (monochrome << 15) | data_size)

        header += struct.pack('<I', ram_offset)

        data += struct.pack('<%sB' % data_size, *self._data)
        return header, data
