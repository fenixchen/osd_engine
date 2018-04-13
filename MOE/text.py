# -*- coding:utf-8 -*-

import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from imageutil import ImageUtil

logger = Log.get_logger("engine")


class Text(Ingredient):
    def __init__(self, window, id, text, color,
                 palette=None,
                 font=None, font_size=None,
                 mutable=False):
        super().__init__(window, id, palette, mutable)
        self._text = text
        self._color = color
        self._font = font
        self._font_size = font_size
        self._glyphs = []

        if isinstance(self._text, int):
            self._text = [self._text]

        for char in self._text:
            glyph = window.get_glyph(char, font, font_size)
            assert glyph is not None
            self._glyphs.append(glyph)

    @property
    def height(self):
        h = 0
        for glyph in self._glyphs:
            h = max(h, glyph.advance_y)
        return h

    @property
    def width(self):
        w = 0
        for glyph in self._glyphs:
            w += glyph.advance_x
        return w

    def top_line(self):
        top = 0xFFFFFFFF
        for glyph in self._glyphs:
            top = min(top, glyph.top)
        return top

    def draw_line(self, window_line_buf, window, y, block_x):
        top = self.top_line()
        color = self.get_color(self._color)
        left = block_x
        for glyph in self._glyphs:
            left += glyph.left
            col = left
            left += glyph.advance_x
            if not 0 <= y - glyph.top + top < glyph.height:
                continue

            offset = (y - glyph.top + top) * glyph.pitch
            for x in range(glyph.width):
                if glyph.monochrome:
                    bit = glyph.data[offset + (x >> 3)]
                    bit = bit & (128 >> (x & 7))
                    if 0 <= col < window.width and bit > 0:
                        window_line_buf[col] = color
                else:
                    intensity = glyph.data[offset + x]
                    if 0 <= col < window.width and intensity > 0:
                        window_line_buf[col] = ImageUtil.blend_pixel(window_line_buf[col], color, intensity)
                col += 1

    def __str__(self):
        return "%s(id:%s, text:%s, color:%d)" % (
            type(self), self.id, self._text, self._color
        )

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.TEXT.value,
                           self._palette.object_index)

        bins += struct.pack('<Bxxx', self._color)

        bins += struct.pack('<xxxx')

        bins += struct.pack('<xxxx')

        return bins, ram
