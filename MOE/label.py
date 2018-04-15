# -*- coding:utf-8 -*-


import struct

from enumerate import *
from ingredient import Ingredient
from log import Log
from text import Text
from block import Block

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, window, id, text, color,
                 width=None, height=None,
                 state=0,
                 background=None,
                 font=None, font_size=None,
                 palette=None,
                 align=Align.LEFT.name,
                 mutable=False):
        super().__init__(window, id, palette, mutable)
        self._width = width
        self._height = height
        self._color = color if type(color) is list else [color]
        self._text = text if type(text) is list else [text]
        self._align = Align[align]
        if background is None:
            self._background = []
        else:
            self._background = background if type(background) is list else [background]

        self._state_count = max(len(self._color), len(self._background))

        for i in range(self._state_count - len(self._color)):
            self._color.append(self._color[0])

        self._ingredient_background = []
        for background in self._background:
            ingredient_bg = window.find_ingredient(background)
            assert ingredient_bg is not None
            ingredient_bg.width = self._width
            ingredient_bg.height = self._height
            self._ingredient_background.append(ingredient_bg)

        if len(self._ingredient_background) > 0:
            for i in range(self._state_count - len(self._background)):
                self._ingredient_background.append(self._ingredient_background[0])

        self._current_state = min(state, self._state_count)
        self._state_blocks = []
        self._text_blocks = []
        self._current_text = 0

        self._ingredient_text = []
        for i, t in enumerate(self._text):
            text = Text(window,
                        "%s_text_%d" % (self.id, i),
                        t,
                        self._color[self._current_state],
                        self._palette.id,
                        font,
                        font_size)
            window.add_ingredient(text)
            self._ingredient_text.append(text)

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
        assert self._current_state < self._state_count

        for i in range(self._state_count):
            state_blocks = [self._color[i]]
            visible = (i == self._current_state)
            if len(self._ingredient_background) > self._current_state:
                bg = self._ingredient_background[i]
                block = Block(window, '', bg, left, top, visible)
                state_blocks.append(block)
                blocks.append(block)
            else:
                state_blocks.append(None)
            self._state_blocks.append(state_blocks)

        for i, text in enumerate(self._ingredient_text):
            if self._height is not None:
                y_delta = int((self._height - text.height) / 2 + 0.5)
            else:
                y_delta = 0

            text_width = text.width
            x_delta = 0
            if self._width is not None and self._width > text_width:
                if self._align == Align.CENTER:
                    x_delta = int((self._width - text_width) / 2)
                elif self._align == Align.RIGHT:
                    x_delta = int(self._width - text_width)

            text_blocks = text.get_blocks(window, block_id,
                                          left + x_delta, top + y_delta, i == 0)
            assert(len(text_blocks) == 1)
            blocks.extend(text_blocks)
            self._text_blocks.append(text_blocks[0])

        return blocks

    @property
    def ingredient_type(self):
        return IngredientType.LABEL.value

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        header = struct.pack('<Bxxx', self.ingredient_type)
        header += struct.pack('<HH', self._state_count, self._current_state)
        header += struct.pack('<HH', len(self._text_blocks), self._current_text)
        header += struct.pack('<I', ram_offset)

        for state_block in self._state_blocks:
            color = state_block[0]
            ram += struct.pack('<I', color)
            bg_block = state_block[1]

            if bg_block is not None:
                ram += struct.pack('<I', bg_block.index)
            else:
                ram += struct.pack('<I', INVALID_BLOCK_INDEX)

        for text_block in self._text_blocks:
            ram += struct.pack('<H', text_block.index)
        return header, ram

    def __str__(self):
        return "%s(id:%s, text(%s), palette:%s)" % (
            type(self), self._id, str(self._text),
            self._palette.id if self._palette is not None else 'None'
        )
