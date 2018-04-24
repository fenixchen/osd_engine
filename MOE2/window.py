# -*- coding:utf-8 -*-

import struct
from log import Log
from font import Font
from palette import Palette
from rectangle import Rectangle
from preload import Preload
from row import Row
from glyph import Glyph

logger = Log.get_logger("engine")


class Window(object):
    def __init__(self, scene, id, x, y, width, height,
                 zorder=0, alpha=255, visible=True,
                 Rows=[],
                 Palettes=[],
                 Rectangles=[],
                 Preloads=[],
                 default_font=None,
                 default_font_size=16):
        self._scene = scene
        self._id = id
        self._scene = scene
        self._x = scene.get_x(x)
        self._y = scene.get_y(y)
        self._width = scene.get_x(width)
        self._height = scene.get_y(height)
        self._zorder = zorder
        self._visible = visible
        self._alpha = alpha
        self._rectangles = []
        self._palettes = []
        self._glyphs = []
        self._texts = []
        self._rows = []
        self._default_font_size = default_font_size

        if default_font is None:
            self._default_font = scene.fonts[0]
        else:
            self._default_font = scene.find_font(default_font)

        for preload_info in Preloads:
            self._create_object(preload_info)

        for palette_info in Palettes:
            palette = self._create_object(palette_info)
            self._palettes.append(palette)

        assert len(self._palettes) == 2

        for rectangle_info in Rectangles:
            rectangle = self._create_object(rectangle_info)
            self._rectangles.append(rectangle)

        for row_info in Rows:
            row = self._create_object(row_info)
            self._rows.append(row)

    def _create_object(self, item):
        assert (len(item.keys()) > 0)
        cls_name = list(item.keys())[0]
        values = item[cls_name]
        logger.debug('Construct Class \'%s\' by %s' % (cls_name, values))
        if cls_name not in globals():
            raise Exception('Undefined class <%s>' % cls_name)
        cls = globals()[cls_name]
        self.scene.format_dict(values)
        obj = cls(window=self, **values)
        logger.debug(obj)
        return obj

    @property
    def default_font(self):
        return self._default_font

    @property
    def default_font_size(self):
        return self._default_font_size

    @property
    def scene(self):
        return self._scene

    @property
    def palettes(self):
        return self._palettes

    def get_kerning(self, font, char1, char2, font_width, font_height):
        if font is None:
            font = self.default_font

        elif isinstance(font, Font):
            font = font
        else:
            font = self._scene.find_font(font)
        return font.get_kerning(char1, char2, font_width, font_height)

    def get_glyph(self, font, char_code, font_width, font_height):
        """
        find glyph, create it if not found
        """
        if font is None:
            font = self.default_font

        elif isinstance(font, Font):
            font = font
        else:
            font = self._scene.find_font(font)

        for glyph in self._glyphs:
            if glyph.char_code == char_code and \
                    glyph.font == font and \
                    glyph.font_width == font_width and \
                    glyph.font_height == font_height:
                return glyph

        glyph = Glyph(font, char_code, font_width, font_height)
        logger.debug(glyph)
        self._glyphs.append(glyph)
        glyph.object_index = len(self._glyphs) - 1
        return glyph

    @property
    def zorder(self):
        return self._zorder

    @property
    def visible(self):
        return self._visible

    @property
    def alpha(self):
        return self._alpha

    @alpha.setter
    def alpha(self, alpha):
        self._alpha = alpha

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, x):
        self._x = x

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, y):
        self._y = y

    @property
    def width(self):
        return self._width

    @width.setter
    def width(self, width):
        self._width = width

    @property
    def height(self):
        return self._height

    @height.setter
    def height(self, height):
        self._height = height

    def get_x(self, block, x):
        if type(x) is int:
            return x
        elif type(x) is float:
            return int(x * self._width)
        raise Exception('Un-supported x <%s>' % x)

    def get_y(self, block, y):
        if type(y) is int:
            return y
        elif type(y) is float:
            return int(y * self._height)
        raise Exception('Un-supported y <%s>' % y)

    def draw_line(self, y):
        painted = False
        window_y = y - self._y
        window_line_buf = [0] * self._width

        for rectangle in self._rectangles:
            if rectangle.y <= window_y < rectangle.y + rectangle.height:
                rectangle.draw_line(window_line_buf, window_y - rectangle.y)
                painted = True

        for row in self._rows:
            if row.y <= window_y < row.y + row.cell_height:
                row.draw_line(window_line_buf, window_y - row.y)
                painted = True

        if painted:
            return window_line_buf
        else:
            return None

    def __str__(self):
        ret = "%s(%s, (%d, %d), %d x %d, zorder:%d)\n" % \
              (type(self), self._id, self._x, self._y, self._width, self._height, self._zorder)
        return ret.rstrip('\n')

    def to_binary(self, ram_offset, ram_usage):
        pass
