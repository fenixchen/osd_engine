# -*- coding:utf-8 -*-

from log import Log
from bitmap import Bitmap
from text import Text
from glyph import Glyph
from image import Image
import struct

logger = Log.get_logger("engine")


class Row(object):
    def __init__(self, window, y, size, x=0, id='',
                 visible=True,
                 Columns=[],
                 show_grid=False):
        self._object_index = None
        self._window = window
        self._y = y
        self._x = x
        self._id = id
        self._visible = visible
        self._cell_width, self._cell_height = size
        self._show_grid = show_grid
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

    @property
    def show_grid(self):
        return self._show_grid

    @show_grid.setter
    def show_grid(self, show):
        self._show_grid = show

    @property
    def id(self):
        return self._id

    @property
    def cell_count(self):
        return self._cell_count

    @property
    def object_index(self):
        assert self._object_index is not None
        return self._object_index

    @object_index.setter
    def object_index(self, i):
        self._object_index = i

    @property
    def visible(self):
        return self._visible

    def is_glyph_row(self):
        for cell in self._cells:
            if cell is not None and cell.is_glyph():
                return True
        return False

    def draw_line(self, window_line_buffer, row_y):
        x = self._x
        for i, cell in enumerate(self._cells):
            if cell is not None:
                cell.draw_line(window_line_buffer,
                               row_y, x, self._cell_width)
                x += self.cell_width + cell.width_delta
            else:
                x += self._cell_width

        if self._show_grid:
            if row_y == 0 or row_y == self._cell_height - 1:
                for x in range(self._x, self._window.width):
                    window_line_buffer[x] = 0xFF00FF
            else:
                x = self._x
                while x < self._window.width:
                    window_line_buffer[x] = 0xFF00FF
                    x += self._cell_width

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

    def to_binary(self, is_glyph_row, resource_addr):
        logger.debug('Generate %s <%s>(y:%d)' % (type(self), self._id, self._y))
        ram = b''
        ram += struct.pack('<HH', self._x, self._y)
        ram += struct.pack('<I', resource_addr)
        flag = (1 if is_glyph_row else 0) | (1 if self._visible else 0) << 1
        ram += struct.pack('<BBBx', flag, self._cell_width, self._cell_height)
        for cell in self._cells:
            if cell is None:
                ram += struct.pack('<BBB', 0, 0, 0)
            else:
                ram += struct.pack('<BBb', cell.object_index, cell.color, cell.width_delta)
        return ram
