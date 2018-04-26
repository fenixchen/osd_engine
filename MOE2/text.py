# -*- coding:utf-8 -*-

import struct
from log import Log
from cell import Cell

logger = Log.get_logger("engine")


class Text(object):
    def __init__(self, row, column, text, color,
                 id='',
                 font=None,
                 font_size=None,
                 mutable=False,
                 width_delta=0):
        self._text = text
        self._color = color
        self._font = font
        self._glyphs = []
        self._window = row.window
        self._palette = row.window.palettes[0]
        self._id = id
        self._row = row
        self._column = column
        self._mutable = mutable
        self._font_size = font_size
        self._width_delta = width_delta
        if self._font_size is None:
            self._font_size = row.window.default_font_size

        if isinstance(self._text, int):
            self._text = [self._text]

    def fill_cells(self, cells):
        column = self._column
        previous = None
        for i, char in enumerate(self._text):
            glyph = self._window.get_glyph(self._font, char,
                                           self._font_size,
                                           self._row.cell_width,
                                           self._row.cell_height)
            assert glyph is not None
            if previous is not None:
                kerning = self._window.get_kerning(self._font, previous, char, self._font_size)
            else:
                kerning = 0
            width_delta = glyph.char_width - self._row.cell_width - kerning
            if i == len(self._text) - 1:
                width_delta += self._width_delta
            cells[column] = Cell(self._row, glyph, self._color, width_delta)
            column += 1
            previous = char

    def __str__(self):
        return "%s(id:%s, text:%s, color:%d)" % (
            type(self), self._id, self._text, self._color
        )

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
