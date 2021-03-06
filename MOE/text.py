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
        top = 0xFF
        for glyph in self._glyphs:
            top = min(top, glyph.top)
        return top

    def draw_line(self, window_line_buf, window, y, block_x):
        top = self.top_line()
        color = self.get_color(self._color)
        left = block_x
        for glyph in self._glyphs:
            col = left + glyph.left
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

    @property
    def ingredient_type(self):
        return IngredientType.TEXT.value

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        header = struct.pack('<BBxx', self.ingredient_type, self._palette.object_index)

        header += struct.pack('<BBH', self._color, self.top_line(), len(self._glyphs))

        font_id = 0
        font_size = 0
        if len(self._glyphs) > 0:
            font_id = self._glyphs[0].font.object_index
            font_size = self._glyphs[0].font_size

        header += struct.pack('<BBxx', font_id, font_size)

        header += struct.pack('<I', ram_offset)

        for glyph in self._glyphs:
            ram += struct.pack('<H', glyph.object_index)

        if len(self._glyphs) % 2 == 1:
            ram += struct.pack('<xx')

        return header, ram
