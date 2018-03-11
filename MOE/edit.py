# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from rectangle import Rectangle
from block import Block

logger = Log.get_logger("engine")

EDIT_TEXT_COLOR = 0

EDIT_BORDER_LOW_LIGHT = 2

EDIT_BORDER_HIGH_LIGHT = 3

EDIT_BORDER_WEIGHT = 1

EDIT_MIN_WIDTH = 30

EDIT_MIN_HEIGHT = 20


class Edit(Ingredient):
    def __init__(self, scene, id, text, font=None, font_size=None,
                 width=None, height=None, palette=None):
        super().__init__(scene, id, palette)
        self._title = Text(scene, text, EDIT_TEXT_COLOR, font, font_size)
        if width is None:
            self._width = max(self._title.width, EDIT_MIN_WIDTH)
        else:
            self._width = width

        if height is None:
            self._height = max(self._title.height, EDIT_MIN_HEIGHT)
        else:
            self._height = height
        border_color = [EDIT_BORDER_LOW_LIGHT,
                        EDIT_BORDER_HIGH_LIGHT,
                        EDIT_BORDER_LOW_LIGHT,
                        EDIT_BORDER_HIGH_LIGHT,
                        ]
        self._rect = Rectangle(scene=scene,
                               id='button_%s_rect' % id,
                               width=self._width,
                               height=self._height,
                               border_color=border_color,
                               border_weight=EDIT_BORDER_WEIGHT,
                               bgcolor=1)
        scene.add_ingredient(self._rect)

    def top_line(self):
        raise Exception("Should never be called")

    def height(self, window):
        raise Exception("Should never be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("Should never be called")

    def get_blocks(self, window, block_id, left, top):
        blocks = [Block(window, '', self._rect, left, top)]
        blocks.extend(self._title.get_blocks(window, block_id, left + 4, top + 4))
        return blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.EDIT.value)
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, text(%s), palette:%s)" % (
            type(self), self._id, str(self._title),
            self._palette.id if self._palette is not None else 'None'
        )
