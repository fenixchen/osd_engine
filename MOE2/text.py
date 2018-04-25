# -*- coding:utf-8 -*-

import struct
from log import Log
from cell import Cell

logger = Log.get_logger("engine")

class Text(object):
    def __init__(self, row, column, id, text, color,
                 font=None,
                 mutable=False):
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

        if isinstance(self._text, int):
            self._text = [self._text]

    def fill_cells(self, cells):
        column = self._column
        previous = None
        for char in self._text:
            glyph = self._window.get_glyph(self._font, char,
                                           self._row.cell_width,
                                           self._row.cell_height)
            assert glyph is not None
            if previous is not None:
                kerning = self._window.get_kerning(self._font, previous, char,
                                                   self._row.cell_width,
                                                   self._row.cell_height)
            else:
                kerning = 0
            width_dec = self._row.cell_width - glyph.width + kerning
            cells[column] = Cell(self._row, glyph, self._color, width_dec)
            column += 1
            previous = char

    def __str__(self):
        return "%s(id:%s, text:%s, color:%d)" % (
            type(self), self._id, self._text, self._color
        )

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))