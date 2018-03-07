# -*- coding:utf-8 -*-


from ingredient import Ingredient
from log import Log
from block import Block

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, scene, id, text, color, font_width, palette=None):
        super().__init__(scene, id, palette)
        self._text = text
        self._color = color
        self._font_width = font_width
        self._glyphs = []
        for char in self._text:
            glyph = scene.get_glyph(char, font_width)
            assert glyph is not None
            glyph.color = color
            self._glyphs.append(glyph)

    def top_line(self):
        raise Exception("Should never be called")

    def height(self, window):
        raise Exception("Should never be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("Should never be called")

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        i = 0
        for glyph in self._glyphs:
            sub_id = '%s_%d' % (block_id, i)
            block = Block(window, sub_id, glyph, left, top)
            blocks.append(block)
            left += glyph.width
            i += 1
        return blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        return b'', b''

    def __str__(self):
        return "%s(id:%s, text:%s, color:%d, font_width:%d, palette:%s)" % (
            type(self),
            self._id, self._text, self._color, self._font_width,
            self._palette.id if self._palette is not None else 'None'
        )
