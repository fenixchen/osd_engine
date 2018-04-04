# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from rectangle import Rectangle
from block import Block

logger = Log.get_logger("engine")

FORM_HEIGHT = 24


class Form(Ingredient):
    def __init__(self, scene, id, title, bgcolor, width=0, height=0,
                 palette=None):
        super().__init__(scene, id, palette)
        self._title = Text(scene, title, 1)
        self._bgcolor = bgcolor
        self._width = width
        self._height = height
        self._title_rect = Rectangle(scene=scene,
                                     id='form_%s_title' % id,
                                     width=self._width,
                                     height=FORM_HEIGHT,
                                     border_color=6,
                                     border_weight=0,
                                     bgcolor=[2, 5],
                                     gradient_mode='LEFT_TO_RIGHT')
        scene.add_ingredient(self._title_rect)

        self._client_rect = Rectangle(scene=scene,
                                      id='form_%s_client' % id,
                                      width=self._width,
                                      height=self._height - FORM_HEIGHT,
                                      border_color=6,
                                      border_weight=0,
                                      bgcolor=self._bgcolor)
        scene.add_ingredient(self._client_rect)

    @property
    def width(self):
        raise Exception("Should never be called")

    @property
    def height(self, window):
        raise Exception("Should never be called")

    @property
    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("Should never be called")

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        blocks.append(Block(window, '', self._title_rect, 0, 0))
        blocks.append(Block(window, '', self._client_rect, 0, FORM_HEIGHT))
        blocks.extend(self._title.get_blocks(window, block_id, 4, 4))
        return blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.FORM.value)
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, title(%s), palette:%s)" % (
            type(self), self._id, str(self._title),
            self._palette.id if self._palette is not None else 'None'
        )
