# -*- coding:utf-8 -*-

import struct

from log import Log
from osdobject import *

logger = Log.get_logger("engine")


class Window(OSDObject):
    def __init__(self, scene, id, x, y, width, height, blocks,
                 zorder=0, alpha=255, visible=True):
        super().__init__(scene, id)
        self._scene = scene
        self._x = scene.get_x(x)
        self._y = scene.get_y(y)
        self._width = scene.get_x(width)
        self._height = scene.get_y(height)
        self._zorder = zorder
        self._visible = visible
        self._blocks = []
        if blocks is not None:
            base_left = 0
            base_top = 0
            for i, block_info in enumerate(blocks):
                block_info = scene.format_tuple(block_info)
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
        self._alpha = alpha

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

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        bins = struct.pack('<BBBB', 0,
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
                block_flags = (1 if block.visible else 0) | (block.index << 1)
                ram += struct.pack('<HHHH', block_flags, block.ingredient.object_index, block.x, block.y)
                i += 1
        return bins, ram
