# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from rectangle import Rectangle
from block import Block

logger = Log.get_logger("engine")

BUTTON_TEXT_FONT_WIDTH = 16

BUTTON_TEXT_COLOR = 0

BUTTON_BORDER_LOW_LIGHT = 2

BUTTON_BORDER_HIGH_LIGHT = 3

BUTTON_BORDER_WEIGHT = 2

BUTTON_BGCOLOR_START = 4

BUTTON_BGCOLOR_END = 5


class Button(Ingredient):
    def __init__(self, scene, id, text,
                 pressed=False,
                 width=None, height=None, palette=None):
        super().__init__(scene, id, palette)
        self._title = Text(scene, text, BUTTON_TEXT_COLOR, BUTTON_TEXT_FONT_WIDTH, False)
        if width is None:
            self._width = self._title.width
        else:
            self._width = width

        if height is None:
            self._height = self._title.height
        else:
            self._height = height
        self._pressed = pressed
        if self._pressed:
            border_color = [BUTTON_BORDER_LOW_LIGHT,
                            BUTTON_BORDER_HIGH_LIGHT,
                            BUTTON_BORDER_LOW_LIGHT,
                            BUTTON_BORDER_HIGH_LIGHT,
                            ]
        else:
            border_color = [BUTTON_BORDER_HIGH_LIGHT,
                            BUTTON_BORDER_LOW_LIGHT,
                            BUTTON_BORDER_HIGH_LIGHT,
                            BUTTON_BORDER_LOW_LIGHT
                            ]
        self._rect = Rectangle(scene=scene,
                               id='button_%s_rect' % id,
                               width=self._width,
                               height=self._height,
                               border_color=border_color,
                               border_weight=BUTTON_BORDER_WEIGHT,
                               bgcolor=[BUTTON_BGCOLOR_START, BUTTON_BGCOLOR_END],
                               gradient_mode='TOP_TO_BOTTOM')
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
        bins = struct.pack('<Bxxx', IngredientType.BUTTON.value)
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, text(%s), palette:%s)" % (
            type(self), self._id, str(self._title),
            self._palette.id if self._palette is not None else 'None'
        )
