# -*- coding:utf-8 -*-

from log import Log
from bitmap import Bitmap
from text import Text
from glyph import Glyph

logger = Log.get_logger("engine")


class Row(object):
    def __init__(self, window, y, size, Columns=[]):
        self._window = window
        self._y = y
        self._cell_width, self._cell_height = size
        self._columns = []
        for column_info in Columns:
            obj = self._create_object(column_info)
            self._columns.append(obj)

        self._cell_count = (window.width + self._cell_width - 1) // self._cell_width
        self._cells = [None] * self._cell_count
        for column in self._columns:
            column.fill_cells(self._cells)

    def _create_object(self, item):
        assert (len(item.keys()) > 0)
        cls_name = list(item.keys())[0]
        values = item[cls_name]
        logger.debug('Construct Class \'%s\' by %s' % (cls_name, values))
        if cls_name not in globals():
            raise Exception('Undefined class <%s>' % cls_name)
        cls = globals()[cls_name]
        self.scene.format_dict(values)
        obj = cls(row=self, **values)
        logger.debug(obj)
        return obj

    def draw_line(self, window_line_buffer, row_y):
        x = 0
        for i, cell in enumerate(self._cells):
            if cell is not None:
                cell.draw_line(window_line_buffer,
                               row_y, x, self._cell_width)
                x += self.cell_width - cell.width_dec
            else:
                x = self._cell_width * i

    @property
    def y(self):
        return self._y

    @property
    def cell_height(self):
        return self._cell_height

    @property
    def cell_width(self):
        return self._cell_width

    @property
    def cell_count(self):
        return self._cell_count

    @property
    def window(self):
        return self._window

    @property
    def scene(self):
        return self._window.scene

    def __str__(self):
        return "%s(y:%d, cell_width:%d, cell_height:%d)" % (
            type(self), self._y, self._cell_width, self._cell_height)
