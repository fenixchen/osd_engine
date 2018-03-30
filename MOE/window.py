# -*- coding:utf-8 -*-

import struct

from log import Log
from osdobject import *

logger = Log.get_logger("engine")


class Window(OSDObject):
    def __init__(self, scene, id, x, y, width, height, blocks,
                 palette=None, zorder=0, alpha=255, visible=True):
        super().__init__(scene, id)
        self._scene = scene
        self._x = scene.get_x(x)
        self._y = scene.get_y(y)
        self._width = scene.get_x(width)
        self._height = scene.get_y(height)
        self._zorder = zorder
        self._visible = visible
        if palette is None:
            self._palette = scene.default_palette
        else:
            self._palette = scene.find_palette(palette)
        self._blocks = []
        if blocks is not None:
            for i, block_info in enumerate(blocks):
                if len(block_info) == 4:
                    (block_id, ingredient_id, left, top) = block_info
                    visible = True
                else:
                    (block_id, ingredient_id, left, top, visible) = block_info
                mutable = len(block_id) > 0
                ingredient = self._scene.find_ingredient(ingredient_id)
                if ingredient is not None:
                    if len(block_id) == 0:
                        block_id = '%s_block_%d' % (self.id, i)
                    blocks = ingredient.get_blocks(self, block_id, left, top)
                    for block in blocks:
                        block.visible = visible
                        block.mutable = mutable
                    self._blocks.extend(blocks)
                else:
                    raise Exception('cannot find ingredient <%s>' % id)
        self._alpha = alpha

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

    @property
    def palette(self):
        return self._palette

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
            if top <= window_y < top + block.height:
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
                block_flags = block.visible
                ram += struct.pack('<HHHH', block_flags, block.ingredient.object_index, block.x, block.y)
                i += 1
        return bins, ram
