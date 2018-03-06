# -*- coding:utf-8 -*-

from plot import Plot
from enumerate import *
from imageutil import ImageUtil
import struct
from log import Log

logger = Log.get_logger("engine")


class Rectangle(Plot):
    def __init__(self, scene, id, width, height, border_color=0,
                 border_weight=0, bgcolor=None,
                 gradient_mode=GradientMode.SOLID.name,
                 border_style=LineStyle.SOLID.name,
                 palette=None):
        super().__init__(scene, id, palette)
        self._width = width
        self._height = height
        self._border_style = LineStyle[border_style]
        if width == "parent":
            self._width = -1
        else:
            self._width = width
        if height == "parent":
            self._height = -1
        else:
            self._height = height
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
            self._gradient_mode = GradientMode.NONE

        if bgcolor is not None:
            if isinstance(bgcolor, int):
                self._bgcolor_start = bgcolor
                self._bgcolor_end = bgcolor
            elif isinstance(bgcolor, list):
                assert len(bgcolor) == 2
                self._bgcolor_start = bgcolor[0]
                self._bgcolor_end = bgcolor[1]
        else:
            self._bgcolor_start = None
            self._bgcolor_end = None

    def _check_border_style(self, x):
        if self._border_style == LineStyle.SOLID:
            return True
        elif self._border_style == LineStyle.DASH:
            return (x % (Plot.DASH_WIDTH + 1)) < Plot.DASH_WIDTH - 1
        elif self._border_style == LineStyle.DOT1:
            return (x % 2) == 0
        elif self._border_style == LineStyle.DOT2:
            return (x % 3) == 0
        elif self._border_style == LineStyle.DOT3:
            return (x % 4) == 0
        elif self._border_style == LineStyle.DASH_DOT:
            index = x % (Plot.DASH_WIDTH + 3)
            return index < Plot.DASH_WIDTH or index == Plot.DASH_WIDTH + 1
        elif self._border_style == LineStyle.DASH_DOT_DOT:
            index = x % (Plot.DASH_WIDTH + 5)
            return index < Plot.DASH_WIDTH or \
                   index == Plot.DASH_WIDTH + 1 or \
                   index == Plot.DASH_WIDTH + 3
        else:
            raise Exception('Unknown border_style <%s>' % self._border_style)

    def _plot_border(self, window_line_buf, window, y, block_x):

        width = window.width if self._width == -1 else self._width
        height = window.height if self._height == -1 else self._height

        if y < self._border_weight:
            # draw top border
            color = self.color(window, self._border_color_top)
            margin = self._border_weight - (self._border_weight - y)
            for x in range(block_x + margin, block_x + width - margin):
                if self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + top border
            color = self.color(window, self._border_color_left)
            for x in range(block_x, block_x + y):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + top border
            color = self.color(window, self._border_color_right)
            for x in range(block_x + width - y, block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color
            return True

        elif y >= height - self._border_weight:
            # draw bottom border
            color = self.color(window, self._border_color_bottom)
            margin = (height - y)
            for x in range(block_x + margin, block_x + width - margin):
                if self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + bottom border
            color = self.color(window, self._border_color_left)
            for x in range(block_x, block_x + height - y):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + bottom border
            color = self.color(window, self._border_color_right)
            for x in range(block_x + width - (height - y), block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            return True

        else:
            color = self.color(window, self._border_color_left)
            for x in range(block_x, block_x + self._border_weight):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            color = self.color(window, self._border_color_right)
            for x in range(block_x + width - self._border_weight, block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color
            return False

    def _fill_rect(self, window_line_buf, window, y, block_x):

        if self._gradient_mode == GradientMode.NONE:
            return

        width = window.width if self._width == -1 else self._width
        height = window.height if self._height == -1 else self._height

        bg_color_start = self.color(window, self._bgcolor_start)
        bg_color_end = self.color(window, self._bgcolor_end)

        color_steps = 1
        if self._gradient_mode == GradientMode.LEFT_TO_RIGHT:
            color_steps = width - self._border_weight * 2
        elif self._gradient_mode == GradientMode.TOP_TO_BOTTOM:
            color_steps = height - self._border_weight * 2
        elif self._gradient_mode == GradientMode.TOP_LEFT_TO_BOTTOM_RIGHT or \
                self._gradient_mode == GradientMode.BOTTOM_LEFT_TO_TOP_RIGHT:
            color_steps = (width - self._border_weight * 2) * (height - self._border_weight * 2)

        r_start, g_start, b_start = ImageUtil.rgb(bg_color_start)
        r_end, g_end, b_end = ImageUtil.rgb(bg_color_end)

        r_delta = (r_end - r_start) / color_steps
        g_delta = (g_end - g_start) / color_steps
        b_delta = (b_end - b_start) / color_steps

        color = bg_color_start

        for x in range(block_x + self._border_weight,
                       block_x + width - self._border_weight):
            if self._gradient_mode == GradientMode.SOLID:
                factor = 0
            elif self._gradient_mode == GradientMode.LEFT_TO_RIGHT:
                factor = x - (block_x + self._border_weight)
            elif self._gradient_mode == GradientMode.TOP_TO_BOTTOM:
                factor = y
            elif self._gradient_mode == GradientMode.TOP_LEFT_TO_BOTTOM_RIGHT:
                factor = (x - block_x + self._border_weight) * y
            elif self._gradient_mode == GradientMode.BOTTOM_LEFT_TO_TOP_RIGHT:
                factor = (x - block_x - self._border_weight) * (height - self._border_weight - y)
            else:
                raise Exception('Unknown gradient mode <%s>' % self._gradient_mode.name)
            color = ImageUtil.color_add(bg_color_start,
                                        r_delta * factor,
                                        g_delta * factor,
                                        b_delta * factor)

            window_line_buf[x] = color

    def plot_line(self, window_line_buf, window, y, block_x):
        is_border = False
        if self._border_weight != 0:
            is_border = self._plot_border(window_line_buf, window, y, block_x)

        if not is_border and self._bgcolor_start != None:
            self._fill_rect(window_line_buf, window, y, block_x)

    def height(self, window):
        if self._height == -1:
            return window.height
        else:
            return self._height

    def __str__(self):
        return "%s(id:%s, (%d x %d)," \
               "border_color(top:%#x, bottom:%#x, left:%#x,right:%#x)," \
               "border_weight:%d, bgcolor:(%#x - %#x), palette:%s)" % (
                   type(self), self._id, self._width, self._height,
                   self._border_color_top, self._border_color_bottom,
                   self._border_color_left, self._border_color_right,
                   self._border_weight,
                   -1 if self._bgcolor_start is None else self._bgcolor_start,
                   -1 if self._bgcolor_end is None else self._bgcolor_end,
                   self._palette.id if self._palette is not None else 'None')

    def to_binary(self, ram_offset):
        logger.debug('Generate rectangle <%s>' % self._id)
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.RECTANGLE.value, self.palette_index())
        bins += struct.pack('<HH',
                            self._width if self._width > 0 else 0xFFFF,
                            self._height if self._height > 0 else 0xFFFF)
        bins += struct.pack('<BBBB', self._border_color_top, self._border_color_bottom, self._border_color_left,
                            self._border_color_right)
        bins += struct.pack('<BBBB', self._border_weight,
                            (self._gradient_mode.value << 4) | self._border_style.value,
                            self._bgcolor_start,
                            self._bgcolor_end)
        return bins, ram
