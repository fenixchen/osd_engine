# -*- coding:utf-8 -*-

import abc

from enumerate import *
from ingredient import Ingredient


class Plot(Ingredient):
    DASH_WIDTH = 10

    def __init__(self, scene, id, palette):
        super().__init__(scene, id, palette)

    def palette_name(self):
        if self._palette is None:
            return "<Parent>"
        else:
            return self._palette.id

    def palette_index(self):
        if self._palette is None:
            return 0xFF
        else:
            return self._palette.object_index

    @abc.abstractmethod
    def plot_line(self, window_line_buf, window, y, block_x):
        raise Exception("must implement by child")

    def draw_line(self, window_line_buf, window, y, block_x):
        self.plot_line(window_line_buf, window, y, block_x)



'''
    def rotate(self, angle):
        cos_val = math.cos(angle / 180)
        sin_val = math.sin(angle / 180)
        x_delta = self._x1 - self._x2
        y_delta = self._y1 - self._y2
        self._x1 = self._x2 + int(cos_val * x_delta - sin_val * y_delta + 0.5)
        self._y1 = self._y2 + int(sin_val * x_delta + cos_val * y_delta + 0.5)

class Circle(Plot):
    def __init__(self, name, color, weight, center_x, center_y, radius, palette=None, bgcolor=None):
        super().__init__(name, palette)
        self._color = color
        self._weight = weight
        self._center_x, self._center_y = center_x, center_y
        self._radius = radius
        self._bgcolor = bgcolor

    def height(self, window):
        return self._radius * 2

    def plot_line(self, window_line_buf, window, y, block_x):
        rx = int((self._radius * self._radius - (self._radius - y) * (self._radius - y)) ** 0.5)
        if not self._bgcolor is None:
            for x in range(self._center_x - rx, self._center_x + rx):
                window_line_buf[x] = self.color(window, self._bgcolor)
        window_line_buf[self._center_x + rx] = self.color(window, self._color)
        window_line_buf[self._center_x - rx] = self.color(window, self._color)

'''
