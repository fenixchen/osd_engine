# -*- coding:utf-8 -*-


# local import
from window import *
from bitmap import *
from modifier import *
from glyph import *
from palette import *
from imageutil import *

logger = Log.get_logger("engine")


class OSDEngine(object):
    def __init__(self, width, height, frame_count):
        self._windows = []
        self._ingredients = []
        self._palettes = []
        self._modifiers = []
        self._width = width
        self._height = height
        self._frame_count = frame_count
        self._frame = Frame(self)

    def __str__(self):
        return "OSDEngine(width: %d, height: %d, frame_count: %d)" % (self.width(), self.height(), self._frame_count)

    def width(self):
        return self._width

    def height(self):
        return self._height

    def disable(self):
        for window in self._windows:
            window.disable()
        for modifier in self._modifiers:
            modifier.disable()

    def enable(self):
        for window in self._windows:
            window.enable()
        for modifier in self._modifiers:
            modifier.enable()

    def windows(self):
        return self._windows

    def ingredients(self):
        return self._ingredients

    def palettes(self):
        return self._palettes

    def modifiers(self):
        return self._modifiers

    def frame_count(self):
        return self._frame_count

    def dump(self):
        logger.debug("宽度: %d, 高度: %d, 帧数: %d" % (self.width(), self.height(), self._frame_count))

        logger.debug("调色板[%d] = {" % len(self._palettes))
        for palette in self._palettes:
            palette.dump()
        logger.debug("}")

        logger.debug("素材[%d] = {" % (len(self._ingredients)))
        for pic in self._ingredients:
            pic.dump()
        logger.debug("}")

        logger.debug("窗口[%d] = {" % len(self._windows))
        for window in self._windows:
            window.dump()
        logger.debug("}")

        logger.debug("修改器[%d] = {" % len(self._modifiers))
        for modifier in self._modifiers:
            modifier.dump()
        logger.debug("}")

    def draw(self, frame_index, painter):
        self._frame.draw(frame_index, painter)


class Frame(object):
    def __init__(self, osd):
        self._osd = osd
        self._width = osd.width()
        self._height = osd.height()
        self._frame_index = 0

    @staticmethod
    def draw_line(y, line_buffer, painter):
        str_color = '{'
        for x, pixel in enumerate(line_buffer.buffer()):
            if pixel == 0:
                str_color = str_color + ' #FFFFFF'
            else:
                str_color = str_color + (" #%06x" % pixel)
        str_color = str_color + '}'
        painter.set_pixel(0, y, str_color)

    def draw(self, frame_index, painter):
        self._frame_index = frame_index
        if frame_index != 0:
            for modifier in self._osd.modifiers():
                if modifier.enabled():
                    modifier.action()
        for y in range(0, self._height):
            window_line_buffers = []
            for window in self._osd.windows():
                if window.enabled() and window.y() <= y < window.y() + window.height():
                    window_line_buffers.append(window.draw_line(y))
            line_buffer = LineBuf(window_line_buffers, self._width)
            self.draw_line(y, line_buffer, painter)
