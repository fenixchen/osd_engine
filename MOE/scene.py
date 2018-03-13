# -*- coding:utf-8 -*-

import os

import hexdump
import yaml

from app import *
from engine import *
from font import Font
from glyph import Glyph
from charactor import Character
from form import Form
from rectangle import Rectangle
from bitmap import Bitmap
from line import Line
from label import Label
from button import Button
from edit import Edit

logger = Log.get_logger("engine")


class Scene(object):
    _BASE_DIR = None

    def __init__(self, yaml_file=None):
        self._yaml_file = None
        self._windows = []
        self._ingredients = []
        self._palettes = []
        self._modifiers = []
        self._glyphs = []
        self._fonts = []
        self._width = -1
        self._height = -1
        self._frames = 1
        self._ticks = 20
        self._timer_ms = 0
        self._title = ''
        self._default_font_size = 16
        self._default_font = None
        self._default_palette = None
        self.load(yaml_file, True)

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
    def filename(self):
        return self._yaml_file

    @property
    def width(self):
        return self._width

    @property
    def height(self):
        return self._height

    @property
    def frames(self):
        return self._frames

    @property
    def ticks(self):
        return self._ticks

    def find_font(self, id):
        for font in self._fonts:
            if font.id == id:
                return font
        raise Exception('Cannot find font <%s>' % id)

    def add_font(self, new_font):
        for font in self._fonts:
            if font.id == new_font.id:
                raise Exception('Duplicated font <%s>' % new_font.id)
        self._fonts.append(new_font)
        new_font.object_index = len(self._fonts) - 1

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
                return ingredient;
        raise Exception('cannot find ingredient <%s>' % id)

    def add_ingredient(self, new_ingredient):
        for ingredient in self._ingredients:
            if ingredient.id == new_ingredient.id:
                raise Exception('Duplicated ingredient <%s>' % new_ingredient.id)
        self._ingredients.append(new_ingredient)
        new_ingredient.object_index = len(self._ingredients) - 1

    def find_window(self, id):
        for window in self._windows:
            if window.id == id:
                return window
        raise Exception('Cannot find window <%s>' % id)

    def add_window(self, new_window):
        for window in self._windows:
            if window.id == new_window.id:
                raise Exception('Duplicated window <%s>' % new_window.id)
        self._windows.append(new_window)
        new_window.object_index = len(self._windows) - 1

    def add_modifier(self, new_modifier):
        for modifier in self._modifiers:
            if modifier.id == new_modifier.id:
                raise Exception('Duplicated modifier <%s>' % new_modifier.id)
        self._modifiers.append(new_modifier)
        new_modifier.object_index = len(self._modifiers) - 1

    def get_character(self, char_code, color, font, font_size):
        '''
        find character, create it if not found
        '''
        if font_size is None:
            font_size = self.default_font_size
        if font is None:
            font = self.default_font
        else:
            font = self.find_font(font)
        for ingredient in self._ingredients:
            if not isinstance(ingredient, Character):
                continue
            character = ingredient
            glyph = character.glyph
            if character.color == color and \
                    glyph._char_code == char_code and \
                    glyph.font == font and \
                    glyph.font_size == font_size:
                return character
        if isinstance(char_code, str):
            code = ord(char_code)
        else:
            code = char_code
        character_id = 'ch_%s_%d_%d_%d' % (font.id, font_size, code, color)

        character = Character(self, character_id, char_code, color, font, font_size)
        logger.debug(character)
        self.add_ingredient(character)
        return character

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
            font = self.find_font(font)

        for glyph in self._glyphs:
            if glyph.char_code == char_code and \
                    glyph.font == font and \
                    glyph.font_size == font_size:
                return glyph

        glyph = Glyph(char_code, font, font_size)
        logger.debug(glyph)
        self._glyphs.append(glyph)
        return glyph

    def find_block(self, id):
        ids = id.split('.')
        if len(ids) == 2:
            window = self.find_window(ids[0])
            if window is not None:
                return window.find_block(ids[1])
        raise Exception('cannot find block <%s>' % id)

    def _set_base_dir(self):
        Scene._BASE_DIR = os.path.dirname(self._yaml_file) + '/'
        ImageUtil.BASE_DIR = Scene._BASE_DIR
        Font.BASE_DIR = Scene._BASE_DIR
        logger.debug('BASE_DIR: <%s>' % Scene._BASE_DIR)

    def load(self, yaml_file, root):
        if not os.path.isfile(yaml_file) or not os.access(yaml_file, os.R_OK):
            raise Exception('%s does not exist or cannot read' % yaml_file)

        if root:
            self._yaml_file = os.path.abspath(yaml_file).replace('\\', '/')
            logger.debug('Loading OSD yaml file: <%s>' % self._yaml_file)
            self._set_base_dir()
        else:
            logger.debug('Loading included OSD yaml file: <%s>' % yaml_file)

        config = None
        with open(yaml_file, "r", encoding='UTF-8') as f:
            content = f.read()
            config = yaml.load(content)

        if config is None:
            raise Exception('cannot load yaml file %s' % self._yaml_file)

        config = config['Scene']
        assert config is not None
        if root:
            self._width = config['width'] if 'width' in config else 640
            self._height = config['height'] if 'height' in config else 480
            self._frames = 1 if 'frames' not in config else int(config['frames'])
            self._title = "" if 'title' not in config else config['title']
            self._title = self._title[:OSD_SCENE_TITLE_MAX_LEN]
            self._timer_ms = 0 if 'timer_ms' not in config else config['timer_ms']
            logger.debug('Title:%s, Width:%d, Height:%d, timer_ms:%d' % (
                self._title, self._width, self._height, self._timer_ms))

        if 'Includes' in config:
            for item in config['Includes']:
                include_file = Scene._BASE_DIR + item['Include']['file']
                self.load(include_file, False)

        if 'Fonts' in config:
            for item in config['Fonts']:
                self.add_font(self._create_object(item))

        if 'Palettes' in config:
            for item in config['Palettes']:
                self.add_palette(self._create_object(item))

        assert len(self._fonts) > 0
        if root:
            if 'default_font' in config:
                self._default_font = self.find_font(config['default_font'])
            else:
                self._default_font = self._fonts[0]

            if 'default_font_size' in config:
                self._default_font_size = int(config['default_font_size'])
            else:
                self._default_font_size = 16

        if 'Ingredients' in config:
            for item in config['Ingredients']:
                self.add_ingredient(self._create_object(item))

        if 'Windows' in config:
            for item in config['Windows']:
                self.add_window(self._create_object(item))

        if 'Modifiers' in config:
            for item in config['Modifiers']:
                self.add_modifier(self._create_object(item))

        logger.debug('OSD YAML file <%s> Loaded' % yaml_file)

        if root:
            self._windows.sort(key=lambda window: window.zorder, reverse=False)

    def _create_object(self, item):
        assert (len(item.keys()) > 0)
        cls_name = list(item.keys())[0]
        values = item[cls_name]
        logger.debug('Construct Class \'%s\' by %s' % (cls_name, values))
        if cls_name not in globals():
            raise Exception('Undefined class <%s>' % cls_name)
        cls = globals()[cls_name]
        obj = cls(scene=self, **values)
        logger.debug(obj)
        return obj

    def modify(self):
        for modifier in self._modifiers:
            if modifier.active:
                modifier.run()

    def paint_line(self, y, line_buffer, painter):
        str_color = '{'
        for pixel in line_buffer:
            r, g, b = ImageUtil.rgb(pixel)
            str_color = str_color + (" #%02x%02x%02x" % (r, g, b))
        str_color = str_color + '}'
        painter.set_pixel(0, y, str_color)

    def merge_line(self, dst_buf, src_buf, src_buf_offset, src_alpha):
        assert (src_buf_offset + len(src_buf) <= self._width)
        for x in range(src_buf_offset, src_buf_offset + len(src_buf)):
            if src_buf[x - src_buf_offset] != 0:
                dst_buf[x] = ImageUtil.blend_pixel(dst_buf[x], src_buf[x - src_buf_offset], src_alpha)

    def draw(self, painter):
        for y in range(0, self._height):
            line_buffer = [0] * self._width
            painted = False
            for window in self._windows:
                if not window.visible:
                    continue
                if window.y <= y < window.y + window.height:
                    window_line_buffer = window.draw_line(y)
                    if window_line_buffer is not None:
                        self.merge_line(line_buffer, window_line_buffer, window.x, window.alpha)
                        painted = True
            if painted:
                self.paint_line(y, line_buffer, painter)

    def __str__(self):
        str = 'Scene(%d x %d, %s)\n' % (self._width, self._height, self._yaml_file)
        str += 'Palettes[%d]\n' % len(self._palettes)
        for palette in self._palettes:
            str += '\t%s\n' % palette
        str += 'Ingredients[%d]\n' % len(self._ingredients)
        for ingredient in self._ingredients:
            str += '\t%s\n' % ingredient
        return str

    def _debug_file(self, filename):
        if False:
            logger.debug('Generate <%s>, size [%d]' % (filename, os.path.getsize(filename)))
            with open(filename, "rb") as f:
                logger.debug('\n' + hexdump.hexdump(f.read(), result='return'))

    def _generate_scene_bin(self, filename, ram_base_addr):
        binary_size = {}
        bins = struct.pack('<HH', self._width, self._height)
        bins += struct.pack('<I', ram_base_addr)
        bins += struct.pack('<BBBB',
                            OSD_GLYPH_HEADER_SIZE, OSD_PALETTE_DATA_SIZE,
                            OSD_INGREDIENT_DATA_SIZE, OSD_WINDOW_DATA_SIZE)

        bins += struct.pack('<%dB' % len(self._title), *self._title.encode('utf-8'))

        bins += struct.pack('<%dx' % (OSD_SCENE_TITLE_MAX_LEN - len(self._title)))

        bins += struct.pack('<Hxx', self._timer_ms)

        with open(filename, "wb") as f:
            f.truncate()
            f.write(bins)
        self._debug_file(filename)
        binary_size[self] = len(bins)

        return binary_size

    def _generate_glyhs_bin(self, ram_filename, ram_base_addr):
        binary_size = {}
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")
        for glyph in self._glyphs:
            ram = glyph.to_binary(ram_base_addr + offset)
            binary_size[glyph] = len(ram)
            ram_file.write(ram)
            offset += len(ram)

        ram_file.close()

        self._debug_file(ram_filename)
        return binary_size

    def _generate_palettes_bin(self, filename, ram_filename, ram_base_addr):
        binary_size = {}
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for palette in self._palettes:
            bins, ram = palette.to_binary(ram_base_addr + offset)
            assert len(bins) == OSD_PALETTE_DATA_SIZE
            binary_size[palette] = len(ram)
            file.write(bins)
            ram_file.write(ram)
            offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)
        return binary_size

    def _generate_ingredients_bin(self, filename, ram_filename, ram_base_addr):
        binary_size = {}
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for ingredient in self._ingredients:
            bins, ram = ingredient.to_binary(ram_base_addr + offset)
            assert len(bins) == OSD_INGREDIENT_DATA_SIZE
            binary_size[ingredient] = len(ram)
            file.write(bins)
            if len(ram) > 0:
                ram_file.write(ram)
                offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)
        return binary_size

    def _generate_windows_bin(self, filename, ram_filename, ram_base_addr):
        binary_size = {}
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for window in self._windows:
            bins, ram = window.to_binary(ram_base_addr + offset)
            assert len(bins) == OSD_WINDOW_DATA_SIZE
            binary_size[window] = len(ram)
            file.write(bins)
            if len(ram) > 0:
                ram_file.write(ram)
                offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)
        return binary_size

    def generate_binary(self, target_folder=None, ram_base_addr=0):
        # prepare target folder
        assert self._yaml_file is not None

        if target_folder is None:
            path = os.path.splitext(self._yaml_file)
            target_folder = path[0] + '.generated/'

        logger.debug('Target folder:%s' % target_folder)

        if not os.path.exists(target_folder):
            os.makedirs(target_folder)

        ram_filename = target_folder + "ram.bin"
        # create empty ram.bin
        with open(ram_filename, "wb") as f:
            f.truncate()

        binary_size = {}
        binary_size.update(self._generate_scene_bin(target_folder + "scene.bin", ram_base_addr))

        binary_size.update(self._generate_glyhs_bin(ram_filename, ram_base_addr))

        binary_size.update(self._generate_palettes_bin(target_folder + "palettes.bin", ram_filename, ram_base_addr))

        binary_size.update(
            self._generate_ingredients_bin(target_folder + "ingredients.bin", ram_filename, ram_base_addr))

        binary_size.update(self._generate_windows_bin(target_folder + "windows.bin", ram_filename, ram_base_addr))

        self._log_binary_size(binary_size)

    def _log_binary_size(self, binary_size):
        total_size = 0

        glyph_size = 0
        for i, glyph in enumerate(self._glyphs):
            s = binary_size[glyph]
            glyph_size += s
            logger.info('[%04d] %-16s (%02d x %02d)=> %d, Total:%d',
                        i, glyph.id, glyph.width, glyph.height, s, glyph_size)
        logger.info('[****] <glyph> size:%d', glyph_size)
        total_size += glyph_size

        palette_size = 0
        for i, palette in enumerate(self._palettes):
            s = binary_size[palette]
            palette_size += s
            logger.info('[%04d] %-32s => %d, Total:%d', i, palette.id, s, palette_size + total_size)
        logger.info('[****] <palette> size:%d', palette_size)
        total_size += palette_size

        ingredient_size = 0
        for i, ingredient in enumerate(self._ingredients):
            s = binary_size[ingredient]
            ingredient_size += s
            logger.info('[%04d] %-32s => %d, Total:%d', i, ingredient.id, s, total_size + ingredient_size)
        logger.info('[****] <ingredient> size:%d', ingredient_size)
        total_size += ingredient_size

        window_size = 0
        for i, window in enumerate(self._windows):
            s = binary_size[window]
            window_size += s
            logger.info('[%04d] %-32s block(%04d) => %d, Total:%d',
                        i, window.id, len(window.blocks), s, window_size + total_size)
        logger.info('[****] <window> size:%d', window_size)
        total_size += window_size

        logger.info('[****] <glyph> size:%d', glyph_size)
        logger.info('[****] <palette> size:%d', palette_size)
        logger.info('[****] <ingredient> size:%d', ingredient_size)
        logger.info('[****] <RAM TOTAL> size:%d', total_size)
