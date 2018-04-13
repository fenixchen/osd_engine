# -*- coding:utf-8 -*-

import struct
from enumerate import *
from log import Log
from osdobject import *
from font import Font
from palette import Palette
from glyph import Glyph
from text import Text
from rectangle import Rectangle
from bitmap import Bitmap
from line import Line
from label import Label
from preload import Preload

logger = Log.get_logger("engine")


class Window(OSDObject):
    def __init__(self, scene, id, x, y, width, height,
                 zorder=0, alpha=255, visible=True,
                 Blocks=[],
                 Palettes=[],
                 Ingredients=[],
                 Preloads=[],
                 default_font=None,
                 default_font_size=16):
        super().__init__(scene, id)
        self._scene = scene
        self._x = scene.get_x(x)
        self._y = scene.get_y(y)
        self._width = scene.get_x(width)
        self._height = scene.get_y(height)
        self._zorder = zorder
        self._visible = visible
        self._alpha = alpha
        self._ingredients = []
        self._palettes = []
        self._glyphs = []
        self._texts = []
        self._default_font_size = default_font_size

        if default_font is None:
            self._default_font = scene._fonts[0]
        else:
            self._default_font = scene.find_font(default_font)

        self._blocks = []

        for preload_info in Preloads:
            preload = self._create_object(preload_info)

        for palette_info in Palettes:
            palette = self._create_object(palette_info)
            self.add_palette(palette)

        assert len(self._palettes) > 0

        self._default_palette = self._palettes[0]

        for ingredient_info in Ingredients:
            ingredient = self._create_object(ingredient_info)
            self.add_ingredient(ingredient)

        base_left = 0
        base_top = 0
        for i, block_info in enumerate(Blocks):
            block_info = scene.format_list(block_info)
            if len(block_info) == 4:
                (block_id, ingredient_id, left, top) = block_info
                visible = True
            else:
                (block_id, ingredient_id, left, top, visible) = block_info
            mutable = len(block_id) > 0

            ingredient = self.find_ingredient(ingredient_id)
            if ingredient is not None:
                if len(block_id) == 0:
                    block_id = '%s_block_%d' % (self.id, i)
                base_left = self._get_coord(base_left, left)
                base_top = self._get_coord(base_top, top)
                blocks = ingredient.get_blocks(self, block_id, base_left, base_top)
                for block in blocks:
                    if block.visible is None:
                        block.visible = visible
                    if block.mutable is None:
                        block.mutable = mutable
                    block.index = len(self._blocks)
                    self._blocks.append(block)
            else:
                raise Exception('cannot find ingredient <%s>' % id)
        self._blocks.sort()

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
    def default_palette(self):
        return self._default_palette

    @property
    def scene(self):
        return self._scene

    def extend_color(self, color_data, palette):
        """
        :param color_data:
        :param palette:
        :return: palette, data_index
        """

        color_set = set()
        for color in color_data:
            color_set.add(color)

        if palette.can_extend(color_set):
            return palette, palette.extend(color_data)

        for pal in self._palettes:
            if pal is palette:
                continue
            if pal.can_extend(color_set):
                return pal, pal.extend(color_data)

        palette = Palette(self,
                          'palette_%d' % (len(self._palettes) + 1),
                          [])
        self.add_palette(palette)
        if palette.can_extend(color_set, 65536):
            return palette, palette.extend(color_data)

        assert False

    def find_palette(self, id):
        for palette in self._palettes:
            if palette.id == id:
                return palette
        raise Exception('Cannot find palette <%s>' % id)

    def add_palette(self, new_palette):
        for palette in self._palettes:
            if palette.id == new_palette.id:
                raise Exception('Duplicated palette <%s>' % new_palette.id)
        self._palettes.append(new_palette)
        new_palette.object_index = len(self._palettes) - 1

    def find_ingredient(self, id):
        for ingredient in self._ingredients:
            if ingredient.id == id:
                return ingredient
        raise Exception('cannot find ingredient <%s>' % id)

    def add_ingredient(self, new_ingredient):
        self._ingredients.append(new_ingredient)
        new_ingredient.object_index = len(self._ingredients) - 1

    def get_glyph(self, char_code, font, font_size):
        """
        find glyph, create it if not found
        """
        if font_size is None:
            font_size = self.default_font_size
        if font is None:
            font = self.default_font
        elif isinstance(font, Font):
            font = font
        else:
            font = self._scene.find_font(font)

        for glyph in self._glyphs:
            if glyph.char_code == char_code and \
                    glyph.font == font and \
                    glyph.font_size == font_size:
                return glyph

        glyph = Glyph(char_code, font, font_size)
        logger.debug(glyph)
        self._glyphs.append(glyph)
        return glyph

    def _get_coord(self, base, value):
        if type(value) is int:
            return value
        elif type(value) is str:
            if value[0] == '+':
                return base + int(value[1:])
            elif value[0] == '-':
                return base - int(value[1:])
        raise Exception('Undefined value string <%s>' % value)

    @property
    def blocks(self):
        return self._blocks

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

    def find_block(self, id):
        for block in self._blocks:
            if block.id == id:
                return block
        return None

    def draw_line(self, y):
        """
        计算window的像素行
        :param y: 行数
        :return: Window 行数据对象
        """
        window_y = y - self._y
        painted = False
        window_line_buf = [0] * self._width
        for block in self._blocks:
            if not block.visible:
                continue
            top = block.top_line()
            block_height = block.height
            if block_height == 0:
                block_height = self._height
            if top <= window_y < top + block_height:
                block.ingredient.draw_line(window_line_buf,
                                           self,
                                           window_y - top,
                                           block.x)
                painted = True
        if painted:
            return window_line_buf
        else:
            return None

    def __str__(self):
        ret = "%s(%s, (%d, %d), %d x %d, zorder:%d)\n" % \
              (type(self), self._id, self._x, self._y, self._width, self._height, self._zorder)
        for block in self._blocks:
            ret += "\t%s @(%d, %d)\n" % (block.ingredient.id, block.x, block.y)
        return ret.rstrip('\n')

    def _stat_ingredient(self):
        bitmap_count = 0
        text_count = 0
        rectangle_count = 0
        line_count = 0
        other_count = 0
        for ingredient in self._ingredients:
            if isinstance(ingredient, Bitmap):
                bitmap_count += 1
            elif isinstance(ingredient, Text):
                text_count += 1
            elif isinstance(ingredient, Rectangle):
                rectangle_count += 1
            elif isinstance(ingredient, Line):
                line_count += 1
            else:
                other_count += 1
        return bitmap_count, text_count, rectangle_count, line_count, other_count

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))

        rams = b''
        bins = b''

        bins += struct.pack('<BBBB',
                            1 if self._visible else 0,
                            self._alpha,
                            self._zorder,
                            len(self._palettes))

        bins += struct.pack('<HH', self._x, self._y)

        bins += struct.pack('<HH', self._width, self._height)

        bitmap_count, text_count, rectangle_count, line_count, other_count = \
            self._stat_ingredient()

        bins += struct.pack('<BBBB', bitmap_count, text_count, rectangle_count, line_count)

        bins += struct.pack('<BBH', other_count, len(self._blocks), len(self._glyphs))

        ram_offset += OSD_PALETTE_HEADER_SIZE * len(self._palettes) + \
                      OSD_INGREDIENT_HEADER_SIZE * len(self._ingredients) + \
                      OSD_BLOCK_HEADER_SIZE * len(self._blocks) + \
                      OSD_GLYPH_HEADER_SIZE * len(self._glyphs)

        # ingredient offset
        if len(self._ingredients) == 0:
            bins += struct.pack('<I', 0)
        else:
            bins += struct.pack('<I', ram_offset)

        # ingredient_offset
        if len(self._palettes) == 0:
            bins += struct.pack('<I', 0)
        else:
            bins += struct.pack('<I', ram_offset)
            palette_header_rams = b''
            palette_data_rams = b''
            for i, palette in enumerate(self._palettes):
                header_ram, data_ram = palette.to_binary(ram_offset)
                assert len(header_ram) == OSD_PALETTE_HEADER_SIZE
                palette_header_rams += header_ram
                palette_data_rams += data_ram
                ram_offset += len(data_ram)

            rams += palette_header_rams
            rams += palette_data_rams

        if len(self._blocks) == 0:
            bins += struct.pack('<I', 0)
        else:
            bins += struct.pack('<I', ram_offset)
            for block in self._blocks:
                header, ram = block.to_binary()
                rams += ram
                ram_offset += len(ram)

        if len(self._glyphs) == 0:
            bins += struct.pack('<I', 0)
        else:
            bins += struct.pack('<I', ram_offset)
            for glyph in self._glyphs:
                ram = glyph.to_binary(ram_offset)
                rams += ram
                ram_offset += len(ram)

        return bins, rams
