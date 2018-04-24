# -*- coding:utf-8 -*-

from cell import Cell


class Image(object):
    def __init__(self, row, column, id, bitmap, width_dec=0,
                 mutable=False):
        self._palette = row.window.palettes[1]
        self._id = id
        self._row = row
        self._column = column
        self._mutable = mutable
        self._window = row.window
        self._width_dec = width_dec
        self._bitmap = row.window.get_bitmap(bitmap)

    def fill_cells(self, cells):
        column = self._column
        cells[column] = Cell(self._row, self._bitmap, None, self._width_dec)

    def __str__(self):
        return "%s(id:%s, bitmap:%s, column:%d)" % \
               (type(self), self._id, self._bitmap.id,
                self._column)
