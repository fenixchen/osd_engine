# -*- coding:utf-8 -*-


import struct

from enumerate import *
from font import Font
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Character(Ingredient):
    def __init__(self, scene, id, char_code, color,
                 font=None, font_size=None, palette=None):

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
            if isinstance(char_code, str):
                code = ord(char_code)
            else:
                code = char_code
            id = "character_%s_%s_%d" % (self._font.id, code, self._font_size)

        super().__init__(scene, id, palette)

        self._glyph = scene.get_glyph(char_code, font, font_size)
        self._color = color

    @property
    def glyph(self):
        return self._glyph

    @property
    def color(self):
        return self._color

    def top_line(self):
        return self._glyph.top

    @property
    def height(self):
        return self._glyph.height

    @property
    def width(self):
        return self._glyph.width

    def draw_line(self, window_line_buf, window, y, block_x):
        glyph = self._glyph

        if not 0 <= y < glyph.height:
            return

        color = self.get_color(self._color)

        width = min(glyph.width, window.width - block_x - glyph.left)

        col = block_x + glyph.left
        offset = y * glyph.pitch
        for x in range(width):
            if glyph.monochrome:
                intensity = glyph.data[offset + (x >> 3)]
                intensity = intensity & (128 >> (x & 7))
                if 0 <= col < window.width and intensity > 0:
                    window_line_buf[col] = color
            else:
                intensity = glyph.data[offset + x]
                if 0 <= col < window.width and intensity > 0:
                    window_line_buf[col] = ImageUtil.blend_pixel(window_line_buf[col], color, intensity)
            col += 1

    def __str__(self):
        ret = "%s(id: %s, font:%s, font_size:%d, char_code:%s, color:%d)" % (
            type(self), self._id, self._font.id, self.glyph.font_size, self.glyph.code, self.color)
        return ret

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.CHARACTER.value,
                           0xFF if self._palette is None else self._palette.object_index)

        bins += struct.pack('<Bxxx', self._color)

        bins += struct.pack('<I', self._glyph.ram_offset)

        bins += struct.pack('<xxxx')

        return bins, ram
