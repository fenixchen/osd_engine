# -*- coding:utf-8 -*-


import struct

from enumerate import *
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Bitmap(Ingredient):
    """
    位图对象
    """

    def width(self):
        return self._width

    def __init__(self, window, id, bitmaps, palette=None,
                 mask_color=None, width=None, height=None, tiled=False,
                 transparent_color=None, mutable=False, current_bitmap = 0):

        super().__init__(window, id, palette, mutable)

        self._data = []
        if isinstance(bitmaps, str):
            bitmaps = [bitmaps]
        assert (isinstance(bitmaps, list))
        self._bitmap_count = len(bitmaps)
        self._width = width
        self._height = height
        self._tiled = tiled
        self._mask_color = mask_color
        self._bitmap_width = 0
        self._bitmap_height = 0
        self._transparent_color = transparent_color
        self._current_bitmap = current_bitmap
        color_data = []
        for bitmap in bitmaps:
            self._bitmap_width, self._bitmap_height, data = ImageUtil.load(bitmap)
            color_data.extend(data)

        assert self._bitmap_width > 0 and self._bitmap_height > 0

        self._palette, self._data = window.extend_color(color_data, self._palette)

        self._current = 0

        assert (len(self._data) == self._bitmap_width * self._bitmap_height * self._bitmap_count)

        if self._width is None:
            self._width = self._bitmap_width

        if self._height is None:
            self._height = self._bitmap_height

    @property
    def height(self):
        if self._tiled:
            return self._height
        else:
            return min(self._height, self._bitmap_height)

    @property
    def width(self):
        if self._tiled:
            return self._width
        else:
            return min(self._width, self._bitmap_width)

    def draw_line(self, window_line_buf, window, y, block_x):
        if y >= self.height:
            return
        if self._tiled:
            width = min(self._width, window.width - block_x)
        else:
            width = min(self._bitmap_width, self._width, window.width - block_x)

        start = self._current * self._bitmap_width * self._bitmap_height \
                + self._bitmap_width * (y % self._bitmap_height)

        cx = start
        for x in range(start, start + width):
            index = self._data[cx]
            col = block_x + x - start
            if self._mask_color is None:
                color = self.get_color(index)
                if 0 <= col < window.width and color != self._transparent_color:
                    window_line_buf[col] = color
            else:
                color = self.get_color(self._mask_color)
                intensity = index
                if 0 <= col < window.width and intensity > 0:
                    window_line_buf[col] = ImageUtil.blend_pixel(window_line_buf[col], color, intensity)

            cx = cx + 1
            if cx - start >= self._bitmap_width:
                cx = start

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
