# -*- coding:utf-8 -*-

import os

import yaml
import hexdump
from app import *
from engine import *
from font import Font
from glyph import Glyph
from window import Window

logger = Log.get_logger("engine")


class Scene(object):
    _BASE_DIR = None

    def __init__(self, yaml_file=None):
        self._yaml_file = None
        self._windows = []
        self._fonts = []
        self._width = 0
        self._height = 0
        self._frames = 1
        self._ticks = 20
        self._timer_ms = 0
        self._title = ''
        self._map_define = {}
        self.load(yaml_file, True)

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

    def get_x(self, x):
        if type(x) is int:
            return x
        elif type(x) is float:
            return int(x * self._width)
        else:
            raise Exception('Un-supported x size:%s' % type(x))

    def get_y(self, y):
        if type(y) is int:
            return y
        elif type(y) is float:
            return int(y * self._height)
        else:
            raise Exception('Un-supported y size:%s' % type(y))

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

    @property
    def fonts(self):
        return self._fonts

    def find_block(self, id):
        ids = id.split('.')
        if len(ids) == 2:
            window = self.find_window(ids[0])
            if window is not None:
                return window.find_block(ids[1])
        raise Exception('cannot find block <%s>' % id)

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
            if 'title' in config:
                self._title = config['title']
            else:
                self._title = os.path.splitext(os.path.basename(yaml_file))[0]
            self._title = self._title[:OSD_SCENE_TITLE_MAX_LEN]
            self._timer_ms = 0 if 'timer_ms' not in config else config['timer_ms']
            logger.debug('Title:%s, Width:%d, Height:%d, timer_ms:%d' % (
                self._title, self._width, self._height, self._timer_ms))

        if 'Defines' in config:
            self._map_define = config['Defines']

        if 'Includes' in config:
            for item in config['Includes']:
                include_file = Scene._BASE_DIR + item['Include']['file']
                self.load(include_file, False)

        if 'Fonts' in config:
            for item in config['Fonts']:
                self.add_font(self._create_object(item))

        if 'Windows' in config:
            for item in config['Windows']:
                self.add_window(self._create_object(item))

        logger.debug('OSD YAML file <%s> Loaded' % yaml_file)

        if root:
            self._windows.sort(key=lambda window: window.zorder, reverse=False)

    def format_var(self, value):
        new_value = value
        if type(value) is str:
            pos = value.find('$')
            if pos >= 0:
                var_name = value[pos + 1:]
                if var_name in self._map_define:
                    if pos > 0:
                        new_value = value[:pos - 1] + self._map_define[var_name]
                    else:
                        new_value = self._map_define[var_name]
                else:
                    raise Exception('Unknown variable <%s>' % value)
        elif type(value) is list:
            new_value = self.format_list(value)
        return new_value

    def format_list(self, values):
        new_values = []
        for value in values:
            new_values.append(self.format_var(value))
        return new_values

    def format_dict(self, values):
        for key, value in values.items():
            values[key] = self.format_var(value)

    def _create_object(self, item):
        assert (len(item.keys()) > 0)
        cls_name = list(item.keys())[0]
        values = item[cls_name]
        logger.debug('Construct Class \'%s\' by %s' % (cls_name, values))
        if cls_name not in globals():
            raise Exception('Undefined class <%s>' % cls_name)
        cls = globals()[cls_name]
        self.format_dict(values)
        obj = cls(scene=self, **values)
        logger.debug(obj)
        return obj

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
        return str

    def _debug_file(self, filename):
        if False:
            logger.debug('Generate <%s>, size [%d]' % (filename, os.path.getsize(filename)))
            with open(filename, "rb") as f:
                logger.debug('\n' + hexdump.hexdump(f.read(), result='return'))

    def _generate_scene_bin(self, binary_size, ram_offset):
        bins = struct.pack('<HH', self._width, self._height)

        bins += struct.pack('<I', ram_offset)

        bins += struct.pack('<BBBB',
                            OSD_PALETTE_HEADER_SIZE, OSD_INGREDIENT_HEADER_SIZE,
                            OSD_WINDOW_HEADER_SIZE, OSD_GLYPH_HEADER_SIZE)

        bins += struct.pack('<BxH', len(self._windows), self._timer_ms);

        bins += struct.pack('<%dB' % len(self._title), *self._title.encode('utf-8'))
        bins += struct.pack('<%dx' % (OSD_SCENE_TITLE_MAX_LEN - len(self._title)))

        binary_size[self] = len(bins)

        return bins

    def _generate_glyhs_bin(self, binary_size, ram_offset):
        rams = b''
        for glyph in self._glyphs:
            ram = glyph.to_binary(ram_offset)
            binary_size[glyph] = len(ram)
            rams += ram
            ram_offset += len(ram)
        return rams

    def _generate_palettes_bin(self, binary_size, ram_offset):
        bins = b''
        rams = b''
        for palette in self._palettes:
            bin, ram = palette.to_binary(ram_offset)
            assert len(bin) == OSD_PALETTE_HEADER_SIZE
            binary_size[palette] = len(ram)
            bins += bin
            rams += ram
            ram_offset += len(ram)

        return bins, rams

    def _generate_ingredients_bin(self, binary_size, ram_offset):
        bins = b''
        rams = b''
        for ingredient in self._ingredients:
            bin, ram = ingredient.to_binary(ram_offset)
            assert len(bin) == OSD_INGREDIENT_HEADER_SIZE
            binary_size[ingredient] = len(ram)
            bins += bin
            rams += ram
            ram_offset += len(ram)

        return bins, rams

    def _generate_windows_bin(self, binary_size, ram_offset):
        bins = b''
        rams = b''
        for window in self._windows:
            bin, ram = window.to_binary(ram_offset)
            assert len(bin) == OSD_WINDOW_HEADER_SIZE
            binary_size[window] = len(ram)
            bins += bin
            rams += ram
            ram_offset += len(ram)

        return bins, rams

    def generate_binary(self, target_binary, target_header, taget_address=0):

        assert self._yaml_file is not None

        binary_size = {}

        ram_offset = taget_address

        ram_offset += OSD_SCENE_HEADER_SIZE

        ram_offset += OSD_WINDOW_HEADER_SIZE * len(self._windows)

        header_size = ram_offset

        scene_bin = self._generate_scene_bin(binary_size, ram_offset)
        assert len(scene_bin) == OSD_SCENE_HEADER_SIZE
        binary_size['scene_bin'] = len(scene_bin)

        window_bin, window_ram = self._generate_windows_bin(binary_size, ram_offset)
        binary_size['window_bin'] = len(window_bin)
        assert len(window_bin) == OSD_WINDOW_HEADER_SIZE * len(self._windows)

        # create empty ram.bin
        with open(target_binary, "wb") as f:
            f.truncate()
            size = f.write(scene_bin)
            size += f.write(window_bin)
            assert size == header_size
            f.write(window_ram)

        # self._log_binary_size(binary_size)

        # self._generate_header_file(target_header, target_binary)

    def _generate_header_file(self, target_header, target_binary):
        # create header files
        with open(target_header, "w+") as f:
            f.truncate()
            a = ''
            define = os.path.splitext(os.path.basename(target_header))[0].upper().strip()
            f.write('#ifndef _SCENE_%s_H_\n' % define)
            f.write('#define _SCENE_%s_H_\n\n' % define)

            f.write("#define OSD_SCENE_%s_TITLE \"%s\"\n" % (define, self._title))
            filename = os.path.basename(target_binary)
            f.write("#define OSD_SCENE_%s_BINARY \"%s\"\n\n" % (define, filename))

            f.write("/* define the macro to open the macros */");

            f.write("\n#ifdef OSD_ENABLE_MACROS_%s \n" % define)

            for i, ingredient in enumerate(self._ingredients):
                if ingredient.mutable and len(ingredient.id) > 0:
                    f.write('#define OSD_INGREDIENT_%-16s %d\n' % (ingredient.id.upper(), i))
            f.write('\n')

            for i, window in enumerate(self._windows):
                f.write('#define OSD_WINDOW_%-16s %d\n' % (window.id.upper(), i))
                for j, block in enumerate(window.blocks):
                    if block.mutable:
                        f.write(
                            '#define OSD_BLOCK_%s_%s %d\n' % (window.id.upper(), block.id.upper(), block.full_index))

            f.write('\n#endif\n\n')

            f.write('\n#endif')

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
        logger.info('[****] <RAM window> size:%d', window_size)
        total_size += window_size

        total_size += binary_size['scene_bin'] + binary_size['palette_bin'] + \
                      binary_size['ingredient_bin'] + binary_size['window_bin']

        logger.info('-' * 32)
        logger.info('[****] <Scene> size:%d' % binary_size['scene_bin'])
        logger.info('[****] <Palette> size:%d' % binary_size['palette_bin'])
        logger.info('[****] <Ingredient> size:%d' % binary_size['ingredient_bin'])
        logger.info('[****] <Window> size:%d' % binary_size['window_bin'])
        logger.info('[****] <Glyph RAM> size:%d' % glyph_size)
        logger.info('[****] <Palette RAM > size:%d' % palette_size)
        logger.info('[****] <Ingredient RAM> size:%d' % ingredient_size)
        logger.info('[****] <Window RAM> size:%d' % window_size)
        logger.info('[****] <Glyph RAM> size:%d' % glyph_size)
        logger.info('[****] <TOTAL> size:%d' % total_size)
