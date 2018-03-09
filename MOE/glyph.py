# -*- coding:utf-8 -*-


import struct

from enumerate import *
from font import Font
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Glyph(Ingredient):

    def __init__(self, scene, id, char, font=None, font_size=None, palette=None, color=None):
        if font is None:
            self._font = scene.default_font
        elif isinstance(font, Font):
            self._font = font
        else:
            self._font = scene.find_font(font)

        if font_size is None:
            self._font_size = scene.default_font_size
        else:
            self._font_size = font_size

        if id is None:
            id = "glyph_%s_%s_%d" % (self._font.id, ord(char), self._font_size)

        super().__init__(scene, id, palette)

        self._left, self._top, self._advance_x, self._advance_y, bitmap = self._font.load_char(char, font_size)
        self._height = bitmap.rows
        self._width = bitmap.width
        self._data = bitmap.buffer[:]
        self._pitch = bitmap.pitch
        self._char = char
        self._color = color

    @property
    def font(self):
        return self._font

    @property
    def char(self):
        return self._char

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
    def color(self):
        return self._color

    @color.setter
    def color(self, c):
        self._color = c

    def top_line(self):
        return self._top

    def height(self, window=None):
        return self._height

    def draw_line(self, line_buf, window, y, block_x):
        if not 0 <= y < self._height:
            return
        color = self.get_color(window, self._color)
        width = min(self._width, window.width - block_x - self._left)
        for x in range(self._pitch * y, self._pitch * y + width):
            index = self._data[x]
            if index == 0:
                continue
            col = block_x + x - self._pitch * y + self._left
            line_buf[col] = ImageUtil.blend_pixel(line_buf[col], color, index)

    def __str__(self):
        ret = "id: %s, left:%d, top:%d, adv:%d, %d x %d, size:%d" % (
            self._id, self._left, self._top, self._advance_x, self._width, self._height, len(self._data))
        return ret

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.GLYPH.value,
                           0xFF if self._palette is None else self._palette.object_index)
        bins += struct.pack('<BBBB', self._left, self._top, self._width, self._height)

        bins += struct.pack('<BBBx', self._pitch, self._color,
                            self._font_size if self._font_size is not None else self.scene.default_font_size)

        data_size = len(self._data)
        bins += struct.pack('<HH', ord(self._char), data_size)

        bins += struct.pack('<I', ram_offset)

        ram = struct.pack('<%sB' % data_size, *self._data)
        return bins, ram
