# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, scene, id, text, color, font=None,
                 font_size=None, vertical=False, palette=None, mutable=False):
        super().__init__(scene, id, palette, mutable)
        self._text = Text(scene, text, color, font, font_size, vertical)
        self._blocks = None

    def top_line(self):
        raise Exception("Should never be called")

    def height(self, window):
        raise Exception("Should never be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("Should never be called")

    def get_blocks(self, window, block_id, left, top):
        if self._blocks is None:
            self._blocks = self._text.get_blocks(window, block_id, left, top)
        return self._blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.LABEL.value)
        if self._mutable:
            bins += struct.pack('<I', len(self._blocks))
            bins += struct.pack('<I', ram_offset)
            bins += struct.pack('<xxxx')

            for block in self._blocks:
                ram += struct.pack('<H', block.ingredient.object_index)
        else:
            bins += struct.pack('<xxxx')
            bins += struct.pack('<xxxx')
            bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, %s, palette:%s)" % (
            type(self), self._id, str(self._text),
            self._palette.id if self._palette is not None else 'None'
        )
