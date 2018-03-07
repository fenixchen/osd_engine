# -*- coding:utf-8 -*-

import os
import shutil

import hexdump
import yaml

from app import *
from engine import *
from font import Font
from rectangle import Rectangle
from line import Line

logger = Log.get_logger("engine")


class Scene(object):
    _BASE_DIR = None

    def __init__(self, yaml_file=None):
        self._yaml_file = None
        self._windows = []
        self._ingredients = []
        self._palettes = []
        self._modifiers = []
        self._width = -1
        self._height = -1
        self._frames = 1
        self._ticks = 20
        self.load(yaml_file)

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

    def find_palette(self, id):
        for palette in self._palettes:
            if palette.id == id:
                return palette
        raise Exception('cannot find palette <%s>' % id)

    def find_ingredient(self, id):
        for ingredient in self._ingredients:
            if ingredient.id == id:
                return ingredient;
        raise Exception('cannot find ingredient <%s>' % id)

    def find_window(self, id):
        for window in self._windows:
            if window.id == id:
                return window
        raise Exception('cannot find window <%s>' % id)

    def find_block(self, id):
        ids = id.split('.')
        if len(ids) == 2:
            window = self.find_window(ids[0])
            if window is not None:
                return window.find_block(ids[1])
        raise Exception('cannot find block <%s>' % id)

    def load(self, yaml_file):
        if not os.path.isfile(yaml_file) or not os.access(yaml_file, os.R_OK):
            raise Exception('%s does not exist or cannot read' % yaml_file)

        self._yaml_file = os.path.abspath(yaml_file).replace('\\', '/')
        logger.debug('Loading OSD yaml file: <%s>' % self._yaml_file)

        Scene._BASE_DIR = os.path.dirname(self._yaml_file) + '/'

        logger.debug('BASE_DIR: <%s>' % Scene._BASE_DIR)

        ImageUtil.BASE_DIR = Scene._BASE_DIR
        Font.BASE_DIR = Scene._BASE_DIR

        self._yaml_file = yaml_file
        with open(self._yaml_file) as f:
            content = f.read()
            config = yaml.load(content)
        if config is None:
            raise Exception('cannot load yaml file %s' % self._yaml_file)

        config = config['OSD']
        assert (config is not None and
                config['width'] is not None and
                config['height'] is not None)
        self._width = config['width']
        self._height = config['height']
        self._frames = 1 if 'frames' not in config else int(config['frames'])

        logger.debug('Width:%d, Height:%d' % (self._width, self._height))

        object_index = 0
        for item in config['Palettes']:
            obj = self._create_object(item)
            self._palettes.append(obj)
            obj.object_index = object_index
            object_index += 1

        object_index = 0
        for item in config['Ingredients']:
            obj = self._create_object(item)
            self._ingredients.append(obj)
            obj.object_index = object_index
            object_index += 1

        object_index = 0
        for item in config['Windows']:
            obj = self._create_object(item)
            self._windows.append(obj)
            obj.object_index = object_index
            object_index += 1
        self.sort_windows()

        if 'Modifiers' in config:
            object_index = 0
            for item in config['Modifiers']:
                obj = self._create_object(item)
                self._modifiers.append(obj)
                obj.object_index = object_index
                object_index += 1

    def sort_windows(self):
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
        assert (src_buf_offset + len(src_buf) < self._width)
        for x in range(src_buf_offset, src_buf_offset + len(src_buf)):
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

    def _generate_global_bin(self, filename, ram_base_addr):
        bins = struct.pack('<HHI', self._width, self._height, ram_base_addr)
        with open(filename, "wb") as f:
            f.truncate()
            f.write(bins)
        self._debug_file(filename)
        return bins

    def _generate_palettes_bin(self, filename, ram_filename, ram_base_addr):
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for palette in self._palettes:
            bins, ram = palette.to_binary(ram_base_addr + offset)
            file.write(bins)
            ram_file.write(ram)
            offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)

    def _generate_ingredients_bin(self, filename, ram_filename, ram_base_addr):
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for ingredient in self._ingredients:
            bins, ram = ingredient.to_binary(ram_base_addr + offset)
            file.write(bins)
            if len(ram) > 0:
                ram_file.write(ram)
                offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)

    def _generate_windows_bin(self, filename, ram_filename, ram_base_addr):
        file = open(filename, "wb")
        offset = os.path.getsize(ram_filename)
        ram_file = open(ram_filename, "ab")

        for window in self._windows:
            bins, ram = window.to_binary(ram_base_addr + offset)
            file.write(bins)
            if len(ram) > 0:
                ram_file.write(ram)
                offset += len(ram)

        file.close()
        ram_file.close()

        self._debug_file(filename)
        self._debug_file(ram_filename)

    def generate_binary(self, target_folder=None, ram_base_addr=0):
        # prepare target folder
        assert self._yaml_file is not None

        if target_folder is None:
            path = os.path.splitext(self._yaml_file)
            target_folder = path[0] + '.generated/'

        logger.debug('Target folder:%s' % target_folder)

        # if os.path.exists(target_folder):
        #    shutil.rmtree(target_folder, ignore_errors=True)

        # os.makedirs(target_folder)

        # create empty ram.bin
        with open(target_folder + "ram.bin", "wb") as f:
            f.truncate()

        self._generate_global_bin(target_folder + "global.bin", ram_base_addr)

        self._generate_palettes_bin(target_folder + "palettes.bin", target_folder + "ram.bin", ram_base_addr)

        self._generate_ingredients_bin(target_folder + "ingredients.bin", target_folder + "ram.bin", ram_base_addr)

        self._generate_windows_bin(target_folder + "windows.bin", target_folder + "ram.bin", ram_base_addr)
