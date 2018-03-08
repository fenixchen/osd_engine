# -*- coding:utf-8 -*-


from ingredient import Ingredient
from log import Log
from block import Block
import struct
from enumerate import *

logger = Log.get_logger("engine")


class Text(object):
    def __init__(self, scene, text, color, font_width, vertical):
        self._scene = scene
        self._text = text
        self._color = color
        self._font_width = font_width
        self._vertical = vertical
        self._glyphs = []
        for char in self._text:
            glyph = scene.get_glyph(char, font_width)
            assert glyph is not None
            glyph.color = color
            self._glyphs.append(glyph)

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        i = 0
        for glyph in self._glyphs:
            sub_id = '%s_%d' % (block_id, i)
            block = Block(window, sub_id, glyph, left, top)
            blocks.append(block)
            if self._vertical:
                top += glyph.advance_y
            else:
                left += glyph.advance_x
            i += 1
        return blocks

    def __str__(self):
        return "text:%s, color:%d, font_width:%d, vertical:%d" % (
            self._text, self._color, self._font_width, self._vertical
        )


class Label(Ingredient):
    def __init__(self, scene, id, text, color, font_width, vertical=False, palette=None):
        super().__init__(scene, id, palette)
        self._text = Text(scene, text, color, font_width, vertical)

    def top_line(self):
        raise Exception("Should never be called")

    def height(self, window):
        raise Exception("Should never be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("Should never be called")

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        blocks.extend(self._text.get_blocks(window, block_id, left, top))
        return blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.LABEL.value)
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, %s, palette:%s)" % (
            type(self), self._id, str(self._text),
            self._palette.id if self._palette is not None else 'None'
        )
