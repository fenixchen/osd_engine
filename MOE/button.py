# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from rectangle import Rectangle
from block import Block

logger = Log.get_logger("engine")


class Button(Ingredient):
    def __init__(self, scene, id, text, color,
                 width, height,
                 bg_rect_normal=None,
                 bg_rect_highlight=None,
                 font=None, font_size=None,
                 palette=None, highlighted=False,
                 mutable=False):

        super().__init__(scene, id, palette, mutable)
        if type(color) is list:
            assert len(color) > 0
            self._color_normal = color[0]
            if len(color) > 1:
                self._color_highlight = color[1]
            else:
                self._color_highlight = color[0]
        else:
            self._color_normal = color
            self._color_highlight = color

        self._text_normal = Text(scene, text, self._color_normal, font, font_size,
                                 width, Align.CENTER.name)

        self._text_highlight = Text(scene, text, self._color_highlight, font, font_size,
                                    width, Align.CENTER.name)

        self._highlighted = highlighted

    @property
    def height(self):
        raise Exception("should not be called")

    @property
    def width(self):
        raise Exception("should not be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("should not be called")

    def get_blocks(self, window, block_id, left, top):
        if self._highlighted:
            return self._text_highlight.get_blocks(window, block_id, left, top)
        else:
            return self._text_normal.get_blocks(window, block_id, left, top)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.BUTTON.value)
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, text(%s), palette:%s)" % (
            type(self), self._id, str(self._title),
            self._palette.id if self._palette is not None else 'None'
        )
