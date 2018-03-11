# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, scene, id, text, color, font=None, font_size=None, vertical=False, palette=None):
        super().__init__(scene, id, palette)
        self._text = Text(scene, text, color, font, font_size, vertical)

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
        return bins, ram

    def __str__(self):
        return "%s(id:%s, %s, palette:%s)" % (
            type(self), self._id, str(self._text),
            self._palette.id if self._palette is not None else 'None'
        )
