# -*- coding:utf-8 -*-

import struct

from enumerate import *
from imageutil import ImageUtil
from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Rectangle(Ingredient):
    def __init__(self, scene, id,
                 width=0, height=0,
                 border_color=0, border_weight=0, bgcolor=None,
                 gradient_mode=GradientMode.SOLID.name,
                 border_style=LineStyle.SOLID.name,
                 palette=None,
                 mutable=False):
        super().__init__(scene, id, palette, mutable)
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

    def _plot_border(self, window_line_buf, window, y, block_x):

        width = window.width if self._width == 0 else self._width
        height = window.height if self._height == 0 else self._height

        if y < self._border_weight:
            # draw top border
            color = self.get_color(window, self._border_color_top)
            margin = self._border_weight - (self._border_weight - y)
            for x in range(block_x + margin, block_x + width - margin):
                if self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + top border
            color = self.get_color(window, self._border_color_left)
            for x in range(block_x, block_x + y):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + top border
            color = self.get_color(window, self._border_color_right)
            for x in range(block_x + width - y, block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color
            return True

        elif y >= height - self._border_weight:
            # draw bottom border
            color = self.get_color(window, self._border_color_bottom)
            margin = (height - y)
            for x in range(block_x + margin, block_x + width - margin):
                if self._check_border_style(x):
                    window_line_buf[x] = color

            # draw left border  + bottom border
            color = self.get_color(window, self._border_color_left)
            for x in range(block_x, block_x + height - y):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            # draw right border  + bottom border
            color = self.get_color(window, self._border_color_right)
            for x in range(block_x + width - (height - y), block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            return True

        else:
            color = self.get_color(window, self._border_color_left)
            for x in range(block_x, block_x + self._border_weight):
                if self._check_border_style(y):
                    window_line_buf[x] = color

            color = self.get_color(window, self._border_color_right)
            for x in range(block_x + width - self._border_weight, block_x + width):
                if self._check_border_style(y):
                    window_line_buf[x] = color
            return False

    def _fill_rect(self, window_line_buf, window, y, block_x):

        if self._gradient_mode == GradientMode.NONE:
            return

        width = window.width if self._width == 0 else self._width
        height = window.height if self._height == 0 else self._height

        bg_color_start = self.get_color(window, self._bgcolor_start)
        bg_color_end = self.get_color(window, self._bgcolor_end)

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

        color = bg_color_start

        for x in range(block_x + self._border_weight,
                       block_x + width - self._border_weight):
            fill_x = x - (block_x + self._border_weight)
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

            window_line_buf[x] = color

    def draw_line(self, window_line_buf, window, y, block_x):
        is_border = False
        if self._border_weight != 0:
            is_border = self._plot_border(window_line_buf, window, y, block_x)

        if not is_border and self._bgcolor_start != None:
            self._fill_rect(window_line_buf, window, y, block_x)

    def top_line(self):
        return 0

    @property
    def height(self):
        return self._height

    @property
    def width(self):
        return self._width

    @width.setter
    def width(self, width):
        self._width = width

    @height.setter
    def height(self, height):
        self._height = height

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
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        ram = b''
        bins = struct.pack('<BBxx', IngredientType.RECTANGLE.value, self.palette_index())
        bins += struct.pack('<HH',
                            self._width if self._width != 0 else 0xFFFF,
                            self._height if self._height != 0 else 0xFFFF)
        bins += struct.pack('<BBBB', self._border_color_top, self._border_color_bottom, self._border_color_left,
                            self._border_color_right)
        bins += struct.pack('<BBBB', self._border_weight,
                            (self._gradient_mode.value << 4) | self._border_style.value,
                            0 if self._bgcolor_start is None else self._bgcolor_start,
                            0 if self._bgcolor_end is None else self._bgcolor_end)

        return bins, ram
