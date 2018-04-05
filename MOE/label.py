# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from rectangle import Rectangle
from block import Block

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, scene, id, text, color,
                 max_chars=0,
                 width=None, height=None,
                 state=0,
                 background=None,
                 font=None, font_size=None,
                 palette=None,
                 align=Align.LEFT.name,
                 mutable=False):
        super().__init__(scene, id, palette, mutable)
        self._width = width
        self._height = height
        self._color = color if type(color) is list else [color]
        self._text = text if type(text) is list else [text]

        if background is None:
            self._background = []
        else:
            self._background = background if type(background) is list else [background]

        self._state_count = max(len(self._color), len(self._text), len(self._background))

        for i in range(self._state_count - len(self._color)):
            self._color.append(self._color[0])

        for i in range(self._state_count - len(self._text)):
            self._text.append(self._text[0])

        self._ingredient_text = []
        for i in range(self._state_count):
            text = Text(scene, self._text[i], self._color[i], font, font_size, self._width, align)
            self._ingredient_text.append(text)

        self._ingredient_background = []
        for background in self._background:
            ingredient_bg = scene.find_ingredient(background)
            assert ingredient_bg is not None
            ingredient_bg.width = self._width
            ingredient_bg.height = self._height
            self._ingredient_background.append(ingredient_bg)

        if len(self._ingredient_background) > 0:
            for i in range(self._state_count - len(self._background)):
                self._ingredient_background.append(self._ingredient_background[0])

        self._state = min(state, self._state_count)
        self._blocks = [[None, None]] * self._state_count

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        raise Exception("should not be called")

    def draw_line(self, line_buf, window, y, block_x):
        raise Exception("should not be called")

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        assert self._state < self._state_count

        self._blocks = []

        for i in range(self._state_count):
            state_blocks = []
            visible = (i == self._state)
            if len(self._ingredient_background) > self._state:
                bg = self._ingredient_background[i]
                block = Block(window, '', bg, left, top, visible)
                state_blocks.append(block)
                blocks.append(block)
            else:
                state_blocks.append(None)

            text = self._ingredient_text[i]
            if self._height is not None:
                y_delta = int((self._height - text.height) / 2 + 0.5)
            else:
                y_delta = 0
            text_blocks = text.get_blocks(window, block_id, left, top + y_delta, visible)
            state_blocks.append(text_blocks)
            blocks.extend(text_blocks)
            self._blocks.append(state_blocks)
        return blocks

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<Bxxx', IngredientType.LABEL.value)
        if self._mutable:
            bins += struct.pack('<HH', self._state_count, self._state)
            bins += struct.pack('<I', ram_offset)
            bins += struct.pack('<xxxx')

            for state_block in self._blocks:
                bg_block = state_block[0]
                char_blocks = state_block[1]
                if bg_block is not None:
                    ram += struct.pack('<I', bg_block.full_index)
                else:
                    ram += struct.pack('<I', INVALID_BLOCK_INDEX)
                ram += struct.pack('<I', len(char_blocks))
                for char in char_blocks:
                    ram += struct.pack('<I', char.full_index)
        else:
            bins += struct.pack('<xxxx')
            bins += struct.pack('<xxxx')
            bins += struct.pack('<xxxx')
        return bins, ram

    def __str__(self):
        return "%s(id:%s, text(%s), palette:%s)" % (
            type(self), self._id, str(self._text),
            self._palette.id if self._palette is not None else 'None'
        )
