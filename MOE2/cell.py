# -*- coding:utf-8 -*-


class Cell(object):
    def __init__(self, row, drawing, color=None, width_dec=0):
        self._row = row
        self._color = color
        self._drawing = drawing
        self._width_dec = width_dec

    @property
    def width_dec(self):
        return self._width_dec

    def draw_line(self, window_line_buffer, row_y, start, width):
        offset = row_y * self._row.cell_width
        for x in range(width):
            color = self._drawing.pixel(offset, self._color)
            if color is not None:
                window_line_buffer[x + start] = color
            offset += 1
