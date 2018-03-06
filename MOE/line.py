# -*- coding:utf-8 -*-

from plot import Plot
from enumerate import *
import struct
from log import Log

logger = Log.get_logger("engine")


class Line(Plot):
    def __init__(self, scene, id, color, weight, x1, y1, x2, y2,
                 style=LineStyle.SOLID.name,
                 palette=None):
        super().__init__(scene, id, palette)
        self._color = color
        self._weight = weight
        self._x1, self._y1, self._x2, self._y2 = x1, y1, x2, y2
        self._style = LineStyle[style]

    def height(self, window):
        if self._y2 == self._y1:
            return self._y2 - self._y1 + 1 + self._weight
        elif self._y2 > self._y1:
            return self._y2 - self._y1 + 1
        else:
            return self._y1 - self._y2 + 1

    def start_y(self):
        return min(self._y1, self._y2)

    def _check_style(self, x):
        if self._style == LineStyle.SOLID:
            return True
        elif self._style == LineStyle.DASH:
            return (x % (Plot.DASH_WIDTH + 1)) < Plot.DASH_WIDTH
        elif self._style == LineStyle.DOT1:
            return (x % 2) == 0
        elif self._style == LineStyle.DOT2:
            return (x % 3) == 0
        elif self._style == LineStyle.DOT3:
            return (x % 4) == 0
        elif self._style == LineStyle.DASH_DOT:
            index = x % (Plot.DASH_WIDTH + 3)
            return index < Plot.DASH_WIDTH or index == Plot.DASH_WIDTH + 1
        elif self._style == LineStyle.DASH_DOT_DOT:
            index = x % (Plot.DASH_WIDTH + 5)
            return index < Plot.DASH_WIDTH or \
                   index == Plot.DASH_WIDTH + 1 or \
                   index == Plot.DASH_WIDTH + 3
        else:
            raise Exception('Unknown style <%s>' % self._style)

    def plot_line(self, window_line_buf, window, y, block_x):
        color = self.color(window, self._color)
        if self._y2 == self._y1:
            if self._y1 <= y < self._y1 + self._weight:
                for x in range(block_x + self._x1, block_x + self._x2):
                    if self._check_style(x):
                        window_line_buf[x] = color
        elif self._x1 == self._x2:
            for i in range(self._weight):
                index = block_x + self._x1 + i
                if self._check_style(y):
                    window_line_buf[index] = color
        else:
            slope = (self._x2 - self._x1) / (self._y2 - self._y1)
            px = self._x1 + int(slope * (y - self._y1) + 0.5)
            for i in range(self._weight):
                if self._check_style(y):
                    window_line_buf[block_x + px + i] = color

    def __str__(self):
        return "%s(id:%s, (%d, %d) - (%d, %x), color:%#x, weight:%d, palette:%s)" % \
               (type(self), self._id, self._x1, self._y1, self._x2, self._y2,
                self._color,
                self._weight,
                self._palette)

    def to_binary(self, ram_offset):
        logger.debug('Generate Line <%s>' % self._id)
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.LINE.value, self.palette_index())
        bins += struct.pack('<HHHH', self._x1, self._y1, self._x2, self._y2)
        bins += struct.pack('<BBBx', self._weight, self._style.value, self._color)
        bins += struct.pack('<xxxx')
        return bins, ram
