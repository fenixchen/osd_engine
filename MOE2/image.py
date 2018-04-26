# -*- coding:utf-8 -*-

from cell import Cell


class Image(object):
    def __init__(self, row, column, bitmap, id='', width_delta=0,
                 mutable=False):
        self._palette = row.window.palettes[1]
        self._id = id
        self._row = row
        if isinstance(column, int):
            self._column = [column]
        elif isinstance(column, list):
            self._column = range(column[0], column[1] + 1)
        else:
            assert False
        self._mutable = mutable
        self._window = row.window
        self._width_delta = width_delta
        self._bitmap = row.window.get_bitmap(bitmap)

    def fill_cells(self, cells):
        for column in self._column:
            cells[column] = Cell(self._row, self._bitmap, None, self._width_delta)

    def __str__(self):
        return "%s(id:%s, bitmap:%s, column:%s)" % \
               (type(self), self._id, self._bitmap.id,
                self._column)
