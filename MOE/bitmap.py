# -*- coding:utf-8 -*-


from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log
import struct

logger = Log.get_logger("engine")


class Bitmap(Ingredient):
    """
    位图对象
    """

    def width(self):
        return self._width

    def __init__(self, scene, id, bitmaps, palette=None):
        super().__init__(scene, id, palette)
        self._data = []
        if isinstance(bitmaps, str):
            bitmaps = [bitmaps]
        assert (isinstance(bitmaps, list))
        self._count = len(bitmaps)
        for bitmap in bitmaps:
            self._width, self._height, data = ImageUtil.load(bitmap)
            self._data.extend(data)
        self._current = 0
        assert (len(self._data) == self._width * self._height * self._count)

    def height(self, window=None):
        return self._height

    def draw_line(self, line_buf, window, y, block_x):
        if 0 <= y < self._height:
            width = min(self._width, window.width - block_x)
            start = self._current * self._width * self._height + self._width * y
            for x in range(start, start + width):
                index = self._data[x]
                line_buf[block_x + x - start] = self.color(window, index)

    def flip(self, loop):
        if not loop and self._current >= self._count - 1:
            return
        self._current = (self._current + 1) % self._count

    def __str__(self):
        return "%s(id: %s, palette: %s, %d x %d, count: %d, len: %d)" % \
               (type(self), self._id, self._palette, self._width, self._height,
                self._count, len(self._data))

    def to_binary(self, ram_offset):
        raise Exception('not implemented')
