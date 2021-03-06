# -*- coding:utf-8 -*-


import struct

from log import Log

logger = Log.get_logger("engine")


class Glyph(object):

    def __init__(self, font, char_code, font_size, font_width, font_height):
        self._data, self._char_width = font.load_char(char_code, font_size, font_width, font_height)
        self._font = font
        self._font_width = font_width
        self._font_height = font_height
        self._char_code = char_code
        self._object_index = None
        self._font_size = font_size

    @property
    def object_index(self):
        assert self._object_index is not None
        return self._object_index

    @object_index.setter
    def object_index(self, i):
        self._object_index = i

    @property
    def monochrome(self):
        return self._monochrome

    @property
    def id(self):
        return '%s-%dx%d-%d' % (self._font.id, self._font_width, self._font_height, self.code)

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
    def width(self):
        return self._font_width

    @property
    def height(self):
        return self._font_height

    @property
    def char_width(self):
        return self._char_width

    @property
    def data(self):
        return self._data

    def pixel(self, offset, color):
        bit = self._data[offset >> 3] & (128 >> (offset & 7))
        if bit > 0:
            return color
        else:
            return None

    def is_glyph(self):
        return True

    def __str__(self):
        ret = "%s(font:%s, font_width:%d, font_height:%d, char:%d, width:%d)" % (
            type(self), self._font.id, self._font_width, self._font_height, self.code,
            self._char_width)
        return ret

    def to_binary(self):
        logger.debug('Generate %s <%s>(%d x %d)' % (type(self), self._char_code, self._font_width, self._font_height))
        data_size = len(self._data)
        return struct.pack('<%sB' % data_size, *self._data)
