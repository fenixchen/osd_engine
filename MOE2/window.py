# -*- coding:utf-8 -*-

import struct
from log import Log
from font import Font
from palette import Palette
from rectangle import Rectangle
from bitmap import Bitmap
from preload import Preload
from row import Row
from glyph import Glyph
from enumerate import *

logger = Log.get_logger("engine")


class Window(object):
    def __init__(self, scene, id, x, y, width, height,
                 zorder=0, alpha=255, visible=True,
                 Palettes=[],
                 Rectangles=[],
                 Bitmaps=[],
                 Preloads=[],
                 Rows=[],
                 default_font=None,
                 default_font_size=None,
                 show_grid=None):
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
        self._bitmaps = []
        self._texts = []
        self._rows = []
        self._default_font_size = default_font_size
        if self._default_font_size is None:
            self._default_font_size = scene.default_font_size

        self._show_grid = show_grid

        if default_font is None:
            self._default_font = scene.fonts[0]
        else:
            self._default_font = scene.find_font(default_font)

        for preload_info in Preloads:
            self._create_object(preload_info)

        for palette_info in Palettes:
            palette = self._create_object(palette_info)
            self._palettes.append(palette)
            if len(self._palettes) == 2:
                continue

        for i in range(len(self._palettes), 2):
            self._palettes.append(Palette(self, 'palette_%d' % i, []))

        assert len(self._palettes) == 2

        for rectangle_info in Rectangles:
            rectangle = self._create_object(rectangle_info)
            self._rectangles.append(rectangle)

        for bitmap_info in Bitmaps:
            bitmap = self._create_object(bitmap_info)
            self._bitmaps.append(bitmap)

        for row_info in Rows:
            row = self._create_object(row_info)
            row.object_index = len(self._rows) - 1
            self._rows.append(row)
            if show_grid is not None:
                row.show_grid = show_grid

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
    def id(self):
        return self._id

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

    def get_kerning(self, font, char1, char2, font_size):
        if font is None:
            font = self.default_font

        elif isinstance(font, Font):
            font = font
        else:
            font = self._scene.find_font(font)
        return font.get_kerning(char1, char2, font_size)

    def get_bitmap(self, bitmap_id):
        for bitmap in self._bitmaps:
            if bitmap.id == bitmap_id:
                return bitmap

        assert False, 'Cannot find bitmap <%s>' % bitmap_id

    def get_glyph(self, font, char_code, font_size, font_width, font_height):
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
                    glyph.font_size == font_size and \
                    glyph.width == font_width and \
                    glyph.height == font_height:
                return glyph

        glyph = Glyph(font, char_code, font_size, font_width, font_height)
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
        window_line_buffer = [0] * self._width

        for rectangle in self._rectangles:
            if not rectangle.visible:
                continue
            if rectangle.y <= window_y < rectangle.y + rectangle.height:
                rectangle.draw_line(window_line_buffer, window_y - rectangle.y)
                painted = True

        for row in self._rows:
            if not row.visible:
                continue
            if row.y <= window_y < row.y + row.cell_height:
                row.draw_line(window_line_buffer, window_y - row.y)
                painted = True

        if self._show_grid:
            painted = True
            if window_y == 0 or window_y == self.height - 1:
                for x in range(self.width):
                    window_line_buffer[x] = 0xFF00FF
            else:
                window_line_buffer[0] = 0xFF00FF
                window_line_buffer[self._width - 1] = 0xFF00FF

        if painted:
            return window_line_buffer
        else:
            return None

    def __str__(self):
        ret = "%s(%s, (%d, %d), %d x %d, zorder:%d)\n" % \
              (type(self), self._id, self._x, self._y, self._width, self._height, self._zorder)
        return ret.rstrip('\n')

    def to_binary(self, ram_offset, ram_usage):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        header = b''
        ram = b''
        header += struct.pack('<BBBx',
                              1 if self._visible else 0,
                              self._alpha,
                              self._zorder)
        header += struct.pack('<HH', self._x, self._y)

        header += struct.pack('<HH', self._width, self._height)

        header += struct.pack('<HH', len(self._rectangles), len(self._rows))

        header += struct.pack('<I', ram_offset)  # rectangle_addr

        # build rectangles
        rectangle_ram = b''
        for rectangle in self._rectangles:
            r = rectangle.to_binary(ram_offset)
            rectangle_ram += r
            ram_usage[rectangle] = len(r)

        assert len(rectangle_ram) == OSD_RECTANGLE_SIZE * len(self._rectangles)
        ram_offset += len(rectangle_ram)
        ram += rectangle_ram

        # build palette0, palette1
        header += struct.pack('<HH',
                              self._palettes[0].count,
                              self._palettes[1].count)

        header += struct.pack('<I', ram_offset)  # palette0_addr
        palette0_ram = self._palettes[0].to_binary()
        ram_usage[self._palettes[0]] = len(palette0_ram)
        assert len(palette0_ram) == self._palettes[0].count * 4
        ram_offset += len(palette0_ram)
        ram += palette0_ram

        header += struct.pack('<I', ram_offset)  # palette1_addr
        palette1_ram = self._palettes[1].to_binary()
        assert len(palette1_ram) == self._palettes[1].count * 4
        ram_usage[self._palettes[1]] = len(palette1_ram)
        ram_offset += len(palette1_ram)
        ram += palette1_ram

        # build glyph
        sized_glyphs = {}

        for glyph in self._glyphs:
            size = (glyph.width, glyph.height)
            if size not in sized_glyphs:
                sized_glyphs[size] = [glyph]
            else:
                sized_glyphs[size].append(glyph)
            glyph.object_index = len(sized_glyphs[size]) - 1

        sized_glyph_addr = {}  # (width, height) => ram_addr
        for sizes in sized_glyphs.keys():
            sized_glyph_addr[sizes] = ram_offset
            glyphs_ram = b''
            for glyph in sized_glyphs[sizes]:
                r = glyph.to_binary()
                glyphs_ram += r
                ram_usage[glyph] = len(r)
            assert len(glyphs_ram) == sizes[0] * sizes[1] / 8 * len(sized_glyphs[sizes])
            ram_offset += len(glyphs_ram)
            ram += glyphs_ram
        logger.debug("sized_glyph_addr ==> %s" % sized_glyph_addr)

        # build bitmaps
        sized_bitmaps = {}
        for bitmap in self._bitmaps:
            size = (bitmap.width, bitmap.height)
            if size not in sized_bitmaps:
                sized_bitmaps[size] = [bitmap]
            else:
                sized_bitmaps[size].append(bitmap)
            bitmap.object_index = len(sized_bitmaps[size]) - 1

        sized_bitmap_addr = {}  # (width, height) => ram_addr
        for sizes in sized_bitmaps.keys():
            bitmap_ram = b''
            sized_bitmap_addr[sizes] = ram_offset
            for bitmap in sized_bitmaps[sizes]:
                r = bitmap.to_binary()
                bitmap_ram += r
                ram_usage[bitmap] = len(r)
            assert len(bitmap_ram) == sizes[0] * sizes[1] * len(sized_bitmaps[sizes])
            ram_offset += len(bitmap_ram)
            ram += bitmap_ram

        logger.debug("sized_bitmap_addr ==>%s" % sized_bitmap_addr)

        header += struct.pack('<I', ram_offset)  # rows_addr
        row_ram = b''
        for row in self._rows:
            if len(row._columns) == 0:
                continue
            sizes = (row.cell_width, row.cell_height)
            is_glyph_row = row.is_glyph_row()
            if is_glyph_row:
                assert sizes in sized_glyph_addr
                resource_addr = sized_glyph_addr[sizes]
            else:
                assert sizes in sized_bitmap_addr
                resource_addr = sized_bitmap_addr[sizes]
            r = row.to_binary(is_glyph_row, resource_addr)
            row_ram += r
            ram_usage[row] = len(r)

        ram_offset += len(row_ram)
        ram += row_ram

        return header, ram
