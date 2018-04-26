# -*- coding:utf-8 -*-

import struct

from enumerate import *
from imageutil import ImageUtil
from log import Log

logger = Log.get_logger("engine")


class Rectangle(object):
    def __init__(self, window,
                 x, y,
                 width, height,
                 id='',
                 border_color=0, border_weight=0,
                 bgcolor=None,
                 gradient_mode=GradientMode.SOLID.name,
                 border_style=LineStyle.SOLID.name,
                 visible=True):
        self._window = window
        self._id = id
        self._x = x
        self._y = y
        self._visible = visible
        self._width = width
        self._height = height
        self._border_style = LineStyle[border_style]
        self._border_weight = border_weight
        if self._border_weight != 0:
            if isinstance(border_color, int):
                self._border_color_top = border_color
                self._border_color_bottom = border_color
                self._border_color_left = border_color
                self._border_color_right = border_color
            elif isinstance(border_color, list):
                assert len(border_color) == 4
                self._border_color_top = border_color[0]
                self._border_color_bottom = border_color[1]
                self._border_color_left = border_color[2]
                self._border_color_right = border_color[3]
            else:
                raise Exception('Unknown border color value <%s>' % border_color)
        else:
            self._border_color_top = 0
            self._border_color_bottom = 0
            self._border_color_left = 0
            self._border_color_right = 0

        if gradient_mode is not None:
            self._gradient_mode = GradientMode[gradient_mode]
        else:
            self._gradient_mode = GradientMode.SOLID

        if bgcolor is not None:
            if isinstance(bgcolor, int):
                self._bgcolor_start = bgcolor
                self._bgcolor_end = bgcolor
            elif isinstance(bgcolor, list):
                assert len(bgcolor) > 0
                if len(bgcolor) == 2:
                    self._bgcolor_start = bgcolor[0]
                    self._bgcolor_end = bgcolor[1]
                elif len(bgcolor) == 1:
                    self._bgcolor_start = bgcolor[0]
                    self._bgcolor_end = bgcolor[0]
        else:
            self._bgcolor_start = None
            self._bgcolor_end = None

        self._palette = window.palettes[0]

    @property
    def visible(self):
        return self._visible

    def _check_border_style(self, x):
        if self._border_style == LineStyle.SOLID:
            return True
        elif self._border_style == LineStyle.DASH:
            return (x % (DASH_WIDTH + 1)) < DASH_WIDTH - 1
        elif self._border_style == LineStyle.DOT1:
            return (x % 2) == 0
        elif self._border_style == LineStyle.DOT2:
            return (x % 3) == 0
        elif self._border_style == LineStyle.DOT3:
            return (x % 4) == 0
        elif self._border_style == LineStyle.DASH_DOT:
            index = x % (DASH_WIDTH + 3)
            return index < DASH_WIDTH or index == DASH_WIDTH + 1
        elif self._border_style == LineStyle.DASH_DOT_DOT:
            index = x % (DASH_WIDTH + 5)
            return index < DASH_WIDTH or \
                   index == DASH_WIDTH + 1 or \
                   index == DASH_WIDTH + 3
        else:
            raise Exception('Unknown border_style <%s>' % self._border_style)

    def get_color(self, color_index):
        return self._palette.color(color_index)

    def _plot_border(self, window_line_buf, y):

        width = self._width
        height = self._height
        window = self._window

        if y < self._border_weight:
            # draw top border
            color = self.get_color(self._border_color_top)
            margin = self._border_weight - (self._border_weight - y)
            for x in range(self._x + margin, self._x + width - margin):
                if 0 <= x < window.width and self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + top border
            color = self.get_color(self._border_color_left)
            for x in range(self._x, self._x + y):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + top border
            color = self.get_color(self._border_color_right)
            for x in range(self._x + width - y, self._x + width):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color
            return True

        elif y >= height - self._border_weight:
            # draw bottom border
            color = self.get_color(self._border_color_bottom)
            margin = (height - y)
            for x in range(self._x + margin, self._x + width - margin):
                if 0 <= x < window.width and self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + bottom border
            color = self.get_color(self._border_color_left)
            for x in range(self._x, self._x + height - y):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + bottom border
            color = self.get_color(self._border_color_right)
            for x in range(self._x + width - (height - y), self._x + width):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color

            return True

        else:
            color = self.get_color(self._border_color_left)
            for x in range(self._x, self._x + self._border_weight):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color

            color = self.get_color(self._border_color_right)
            for x in range(self._x + width - self._border_weight, self._x + width):
                if 0 <= x < window.width and self._check_border_style(y):
                    window_line_buf[x] = color
            return False

    def _fill_rect(self, window_line_buf, y):

        if self._gradient_mode == GradientMode.NONE:
            return

        width = self._width
        height = self._height

        bg_color_start = self.get_color(self._bgcolor_start)
        bg_color_end = self.get_color(self._bgcolor_end)

        color_steps = 1
        fill_width = width - self._border_weight * 2
        fill_height = height - self._border_weight * 2
        if self._gradient_mode == GradientMode.LEFT_TO_RIGHT:
            color_steps = fill_width
        elif self._gradient_mode == GradientMode.TOP_TO_BOTTOM:
            color_steps = fill_height
        elif self._gradient_mode == GradientMode.TOP_LEFT_TO_BOTTOM_RIGHT or \
                self._gradient_mode == GradientMode.BOTTOM_LEFT_TO_TOP_RIGHT:
            color_steps = fill_width * fill_height
        elif self._gradient_mode == GradientMode.CORNER_TO_CENTER:
            color_steps = (fill_width * fill_height) >> 2

        r_start, g_start, b_start = ImageUtil.rgb(bg_color_start)
        r_end, g_end, b_end = ImageUtil.rgb(bg_color_end)

        r_delta = (r_end - r_start) / color_steps
        g_delta = (g_end - g_start) / color_steps
        b_delta = (b_end - b_start) / color_steps

        for x in range(self._x + self._border_weight,
                       self._x + width - self._border_weight):
            fill_x = x - (self._x + self._border_weight)
            fill_y = y - self._border_weight
            if self._gradient_mode == GradientMode.SOLID:
                factor = 0
            elif self._gradient_mode == GradientMode.LEFT_TO_RIGHT:
                factor = fill_x
            elif self._gradient_mode == GradientMode.TOP_TO_BOTTOM:
                factor = y
            elif self._gradient_mode == GradientMode.TOP_LEFT_TO_BOTTOM_RIGHT:
                factor = fill_x * fill_y
            elif self._gradient_mode == GradientMode.BOTTOM_LEFT_TO_TOP_RIGHT:
                factor = fill_x * (fill_height - fill_y)
            elif self._gradient_mode == GradientMode.CORNER_TO_CENTER:
                if fill_x <= (fill_width >> 1):
                    factor_x = fill_x
                else:
                    factor_x = fill_width - fill_x

                if fill_y <= (fill_height >> 1):
                    factor_y = fill_y
                else:
                    factor_y = (fill_height - fill_y)
                factor = factor_x * factor_y
            else:
                raise Exception('Unknown gradient mode <%s>' % self._gradient_mode.name)
            color = ImageUtil.color_add(bg_color_start,
                                        r_delta * factor,
                                        g_delta * factor,
                                        b_delta * factor)
            if 0 <= x < self._window.width:
                window_line_buf[x] = color

    def draw_line(self, window_line_buf, y):
        is_border = False
        if self._border_weight != 0:
            is_border = self._plot_border(window_line_buf, y)

        if not is_border and self._bgcolor_start is not None:
            self._fill_rect(window_line_buf, y)

    @property
    def id(self):
        return self._id

    @property
    def x(self):
        return self._x

    @property
    def y(self):
        return self._y

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        return self._width

    def __str__(self):
        return "%s(id:%s, (%d x %d)," \
               "border_color(top:%#x, bottom:%#x, left:%#x,right:%#x)," \
               "border_weight:%d, bgcolor:(%#x - %#x)" % (
                   type(self), self._id, self._width, self._height,
                   self._border_color_top, self._border_color_bottom,
                   self._border_color_left, self._border_color_right,
                   self._border_weight,
                   -1 if self._bgcolor_start is None else self._bgcolor_start,
                   -1 if self._bgcolor_end is None else self._bgcolor_end)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        ram = b''

        ram += struct.pack('<HH', self._x, self._y)

        ram += struct.pack('<HH', self._width, self._height)

        ram += struct.pack('<BBBB', self._border_color_top,
                           self._border_color_bottom,
                           self._border_color_left,
                           self._border_color_right)

        assert self._border_weight <= 0x7F
        if self._visible:
            border_weight = self._border_weight | (1 << 7)
        else:
            border_weight = self._border_weight
        ram += struct.pack('<BBBB', border_weight,
                           (self._gradient_mode.value << 4) | self._border_style.value,
                           0 if self._bgcolor_start is None else self._bgcolor_start,
                           0 if self._bgcolor_end is None else self._bgcolor_end)

        return ram
