# -*- coding:utf-8 -*-


class Cell(object):
    def __init__(self, row, glyph, color, width_dec):
        self._row = row
        self._glyph = glyph
        self._color = color
        self._width_dec = width_dec

    @property
    def width_dec(self):
        return self._width_dec

    def draw_line(self, window_line_buffer, row_y, start, width):
        bit_offset = row_y * self._row.cell_width
        for x in range(width):
            if self._glyph.bit(bit_offset) > 0:
                window_line_buffer[x + start] = self._color
            bit_offset += 1
