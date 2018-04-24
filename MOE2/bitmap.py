# -*- coding:utf-8 -*-


import struct

from enumerate import *
from imageutil import ImageUtil
from log import Log
from cell import Cell

logger = Log.get_logger("engine")


class Bitmap(object):
    """
    位图对象
    """

    def width(self):
        return self._width

    def __init__(self, row, column, id, bitmaps,
                 width=None, height=None, tiled=False,
                 transparent_color=None, mutable=False, current_bitmap=0,
                 width_dec=0):

        self._palette = row.window.palettes[1]
        self._row = row
        self._column = column
        self._id = id
        self._multable = False

        self._data = []
        if isinstance(bitmaps, str):
            bitmaps = [bitmaps]
        assert (isinstance(bitmaps, list))
        self._bitmap_count = len(bitmaps)
        self._width = row.cell_width
        self._height = row.cell_height
        self._width_dec = width_dec
        self._transparent_color = transparent_color
        self._current_bitmap = current_bitmap
        color_data = []
        for bitmap in bitmaps:
            data = ImageUtil.load(bitmap, self._width, self._height, transparent_color)
            color_data.extend(data)

        self._data = self._palette.extend(color_data, transparent_color)

        self._current = 0

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        return self._width

    def fill_cells(self, cells):
        column = self._column
        cells[column] = Cell(self._row, self, None, self._width_dec)

    def pixel(self, pixel_offset, color):
        start = self._width * self._height * self._current_bitmap
        start += pixel_offset
        color_index = self._data[pixel_offset]
        color = self._palette.color(color_index)
        if (color & 0xFF000000) == 0xFF000000:
            return None
        else:
            return color

    def __str__(self):
        return "%s(id: %s, palette: %s, %d x %d, count: %d, len: %d)" % \
               (type(self), self._id, self._palette, self._width, self._height,
                self._bitmap_count, len(self._data))

    @property
    def ingredient_type(self):
        return IngredientType.BITMAP.value

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        ram = b''
        pixel_bits = self._palette.pixel_bits
        headers = struct.pack('<BBxx', self.ingredient_type, self._palette.object_index)

        flag = (1 if self._tiled else 0) | \
               (0 if self._transparent_color is None else 1) << 1

        headers += struct.pack('<BBBB',
                               flag,
                               0 if self._mask_color is None else self._mask_color,
                               self._bitmap_count,
                               self._current_bitmap)

        headers += struct.pack('<I', ram_offset)

        headers += struct.pack('<xxxx')

        data_size = len(self._data)

        ram += struct.pack('<HH', self._bitmap_width, self._bitmap_height)

        ram += struct.pack('<I', 0 if self._transparent_color is None else self._transparent_color)

        ram += struct.pack('<I', data_size)

        if pixel_bits <= 8:
            ram += struct.pack('<%sB' % data_size, *self._data)
        elif pixel_bits <= 16:
            ram += struct.pack('<%sH' % data_size, *self._data)
        else:
            raise Exception('Not implemented %d' % pixel_bits)
        return headers, ram
