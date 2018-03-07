# -*- coding:utf-8 -*-

from log import Log
from osdobject import *
import struct
from block import Block

logger = Log.get_logger("engine")

class Window(OSDObject):
    def __init__(self, scene, id, x, y, width, height, palette, blocks,
                 zorder=0, alpha=255, visible=True):
        self._scene = scene
        self._id = id
        self._x = x
        self._y = y
        self._width = width
        self._height = height
        self._zorder = zorder
        self._visible = visible
        self._palette = scene.find_palette(palette)
        self._blocks = []
        for (block_id, id, left, top) in blocks:
            ingredient = self._scene.find_ingredient(id)
            if ingredient is not None:
                self._blocks.extend(ingredient.get_blocks(self, block_id, left, top))
            else:
                raise Exception('cannot find ingredient <%s>' % id)
        self._alpha = alpha

    @property
    def id(self):
        return self._id

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

    @property
    def palette(self):
        return self._palette

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
            top = block.top_line() + block.y
            if top <= window_y < top + block.height(self):
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

    def palette_index(self):
        if self._palette is None:
            return 0xFF
        else:
            return self._palette.object_index

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        bins = struct.pack('<BBBB', self.palette_index(),
                           1 if self._visible else 0,
                           self._alpha,
                           self._zorder)
        bins += struct.pack('<HHHH', self._x, self._y, self._width, self._height)
        bins += struct.pack('<I', len(self._blocks))
        ram = b''
        if len(self._blocks) == 0:
            bins += struct.pack('<I', 0)
        else:
            bins += struct.pack('<I', ram_offset)
            i = 0
            for block in self._blocks:
                ram += struct.pack('<HHHH', i, block.ingredient.object_index, block.x, block.y)
                i += 1
        return bins, ram
