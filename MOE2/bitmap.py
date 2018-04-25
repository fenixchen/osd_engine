# -*- coding:utf-8 -*-


import struct

from enumerate import *
from imageutil import ImageUtil
from log import Log
from cell import Cell

logger = Log.get_logger("engine")


class Bitmap(object):
    def __init__(self, window, id, bitmaps,
                 transparent_color=None,
                 current_bitmap=0):
        self._object_index = None
        self._palette = window.palettes[1]
        self._window = window
        self._id = id

        self._data = []
        if isinstance(bitmaps, str):
            bitmaps = [bitmaps]
        assert (isinstance(bitmaps, list))
        self._bitmap_count = len(bitmaps)
        self._transparent_color = transparent_color
        self._current_bitmap = current_bitmap
        color_data = []
        for bitmap in bitmaps:
            self._width, self._height, data = ImageUtil.load(bitmap, transparent_color)
            color_data.extend(data)

        self._data = self._palette.extend(color_data, transparent_color)

        self._current = 0

    @property
    def object_index(self):
        assert self._object_index is not None
        return self._object_index

    @object_index.setter
    def object_index(self, i):
        self._object_index = i

    @property
    def id(self):
        return self._id

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        return self._width

    def pixel(self, pixel_offset, color):
        start = self._width * self._height * self._current_bitmap
        start += pixel_offset
        color_index = self._data[pixel_offset]
        color = self._palette.color(color_index)
        if ImageUtil.alpha(color) == 0xFF:
            return None
        else:
            return color

    def is_glyph(self):
        return False

    def __str__(self):
        return "%s(id: %s, palette: %s, %d x %d, count: %d, len: %d)" % \
               (type(self), self._id, self._palette, self._width, self._height,
                self._bitmap_count, len(self._data))

    def to_binary(self):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        data_size = len(self._data)
        return struct.pack('<%sB' % data_size, *self._data)