# -*- coding:utf-8 -*-


class Cell(object):
    def __init__(self, row, drawing, color=None, width_delta=0):
        self._row = row
        self._color = color
        self._drawing = drawing
        self._width_delta = width_delta

    @property
    def width_delta(self):
        return self._width_delta

    def is_glyph(self):
        return self._drawing.is_glyph()

    @property
    def object_index(self):
        return self._drawing.object_index

    @property
    def color(self):
        return 0 if self._color is None else self._color

    def draw_line(self, window_line_buffer, row_y, start, width):
        offset_start = row_y * self._drawing.width
        if self._color is not None:
            color = self._row.window.palettes[0].color(self._color)
        else:
            color = None
        offset = 0
        for x in range(width):
            if x >= self._row.cell_width + self._width_delta:
                continue
            draw_color = self._drawing.pixel(offset_start + offset, color)
            if draw_color is not None:
                window_line_buffer[x + start] = draw_color
            offset = (offset + 1) % self._drawing.width
