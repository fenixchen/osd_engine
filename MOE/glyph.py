# -*- coding:utf-8 -*-


import struct

from enumerate import *
from font import Font
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Glyph(object):

    def __init__(self, char_code, font, font_size):
        self._left, self._top, self._advance_x, self._advance_y, self._monochrome, bitmap = \
            font.load_char(char_code, font_size)
        self._font = font
        self._font_size = font_size
        self._height = bitmap.rows
        self._width = bitmap.width
        self._pitch = bitmap.pitch
        self._char_code = char_code
        self._ram_offset = None
        if self._monochrome and self._pitch * 8 - self._width > 8:
            self._data = []
            new_pitch = (self._width + 7) // 8
            for y in range(self._height):
                for x in range(new_pitch):
                    self._data.append(bitmap.buffer[self.pitch * y + x])
            self._pitch = new_pitch
        else:
            self._data = bitmap.buffer[:]

    @property
    def monochrome(self):
        return self._monochrome

    @property
    def id(self):
        return '%s-%d-%d' % (self._font.id, self._font_size, self.code)

    @property
    def ram_offset(self):
        assert self._ram_offset is not None
        return self._ram_offset

    @property
    def font(self):
        return self._font

    @property
    def font_size(self):
        return self._font_size

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
    def font_size(self):
        return self._font_size

    @property
    def advance_x(self):
        return self._advance_x

    @property
    def advance_y(self):
        return self._advance_y

    @property
    def left(self):
        return self._left

    @property
    def top(self):
        return self._top

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        return self._width

    @property
    def pitch(self):
        return self._pitch

    @property
    def data(self):
        return self._data

    def __str__(self):
        ret = "%s(font:%s, font_size:%d, left:%d, char:%d, top:%d, pitch:%d, adv(%d,%d), %d x %d, size:%d)" % (
            type(self), self._font.id, self._font_size, self.code,
            self._left, self._top, self._pitch, self._advance_x, self._advance_y,
            self._width, self._height, len(self._data))
        return ret

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s-%s-%d>' % (
            type(self), self._font.id, self.char_code, self._font_size))

        self._ram_offset = ram_offset

        ram = struct.pack('<BBBB', self._left, self._top, self._width, self._height)

        ram += struct.pack('<HBB', self.code, self._font.object_index, self._font_size)

        data_size = len(self._data)
        assert (data_size <= 0xFFFF)

        monochrome = 1 if self._monochrome else 0

        ram += struct.pack('<BBH', self._pitch, self.advance_x,
                           (monochrome << 15) | data_size)

        ram += struct.pack('<%sB' % data_size, *self._data)
        return ram
