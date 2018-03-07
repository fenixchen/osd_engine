# -*- coding:utf-8 -*-


from font import Font
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log
import struct
from enumerate import *

FONT = Font()

logger = Log.get_logger("engine")


class Glyph(Ingredient):

    def __init__(self, scene, id, font_width, char, height=-1, palette=None, color=None):
        super().__init__(scene, id, palette)
        self._left, self._top, bitmap = FONT.load_char(char, font_width)
        self._height = bitmap.rows
        self._width = bitmap.width
        self._data = bitmap.buffer[:]
        self._pitch = bitmap.pitch
        self._char = char
        self._font_width = font_width
        self._color = color

    @property
    def char(self):
        return self._char

    @property
    def width(self):
        return self._width

    def top_line(self):
        return self._top

    def height(self, window=None):
        return self._height + self._top

    def draw_line(self, line_buf, window, y, block_x):
        assert (0 <= y < self._height)
        color = self.color(window, self._color)
        width = min(self._width, window.width - block_x)
        for x in range(self._pitch * y, self._pitch * y + width):
            index = self._data[x]
            if index == 0:
                continue
            col = block_x + x - self._pitch * y
            line_buf[col] = ImageUtil.blend_pixel(line_buf[col], color, index)

    def __str__(self):
        ret = "id: %s, %d x %d, size: %d" % (self._id, self._width, self._height, len(self._data))
        return ret

    def to_binary(self, ram_offset):
        logger.debug('Generate Bitmap <%s>' % self._id)
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.GLYPH.value,
                           0xFF if self._palette is None else self._palette.object_index)
        bins += struct.pack('<BBBB', self._left, self._top, self._width, self._height)
        bins += struct.pack('<BBBx', self._pitch, self._color, self._font_width)

        data_size = len(self._data)
        bins += struct.pack('<HH', ord(self._char), data_size)

        bins += struct.pack('<I', ram_offset)

        ram = struct.pack('<%sB' % data_size, *self._data)
        return bins, ram
