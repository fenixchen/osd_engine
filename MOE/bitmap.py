# -*- coding:utf-8 -*-


from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log
import struct
from enumerate import *

logger = Log.get_logger("engine")


class Bitmap(Ingredient):
    """
    位图对象
    """

    def width(self):
        return self._width

    def __init__(self, scene, id, bitmaps, palette):
        super().__init__(scene, id, palette)
        assert self._palette is not None
        self._data = []
        if isinstance(bitmaps, str):
            bitmaps = [bitmaps]
        assert (isinstance(bitmaps, list))
        self._bitmap_count = len(bitmaps)
        color_data = []
        for bitmap in bitmaps:
            self._width, self._height, data = ImageUtil.load(bitmap)
            color_data.extend(data)
        for color in color_data:
            self._data.append(self._palette.get_color_index(color))
        self._current = 0

        assert (len(self._data) == self._width * self._height * self._bitmap_count)

    def height(self, window=None):
        return self._height

    def draw_line(self, line_buf, window, y, block_x):
        if 0 <= y < self._height:
            width = min(self._width, window.width - block_x)
            start = self._current * self._width * self._height + self._width * y
            for x in range(start, start + width):
                index = self._data[x]
                color = self.color(window, index)
                line_buf[block_x + x - start] = color

    def flip(self, loop):
        if not loop and self._current >= self._bitmap_count - 1:
            return
        self._current = (self._current + 1) % self._bitmap_count

    def __str__(self):
        return "%s(id: %s, palette: %s, %d x %d, count: %d, len: %d)" % \
               (type(self), self._id, self._palette, self._width, self._height,
                self._bitmap_count, len(self._data))

    def to_binary(self, ram_offset):
        logger.debug('Generate Bitmap <%s>' % self._id)
        ram = b''
        pixel_bits = self._palette.pixel_bits
        bins = struct.pack('<BBxx', IngredientType.BITMAP.value, self._palette.object_index)
        bins += struct.pack('<HH', self._width, self._height)
        data_size = int(len(self._data) * pixel_bits / 8)
        bins += struct.pack('<BxH', self._bitmap_count, data_size)
        bins += struct.pack('<I', ram_offset)
        if pixel_bits == 8:
            ram = struct.pack('<%sB' % data_size, *self._data)
        else:
            raise Exception('Not implemented %d' % pixel_bits)
        return bins, ram
