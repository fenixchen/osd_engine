# -*- coding:utf-8 -*-

import abc

from block import Block
from osdobject import OSDObject


class Ingredient(OSDObject):

    def __init__(self, window, id, palette, mutable=False):
        super().__init__(window.scene, id)
        if palette is None:
            self._palette = window.default_palette
        else:
            self._palette = window.find_palette(palette)
        self._mutable = mutable

    @property
    def mutable(self):
        return self._mutable

    @property
    def palette(self):
        return self._palette

    def palette_index(self):
        return self._palette.object_index

    def get_color(self, color_index):
        assert self._palette is not None
        return self._palette.color(color_index)

    def get_blocks(self, window, block_id, left, top, visible = True):
        block = Block(window, block_id, self, left, top, visible)
        return block,

    def top_line(self):
        return 0

    @property
    @abc.abstractmethod
    def height(self):
        raise Exception("must implemented by child")

    @property
    @abc.abstractmethod
    def width(self):
        raise Exception("must implemented by child")

    @abc.abstractmethod
    def draw_line(self, line_buf, window, y, block_x):
        """
        在窗口中绘图
        :param window:窗口
        :param y:自己的第几行
        :param block_x: 当前Block在window中的x位置
        """
        raise Exception("must implemented by child")
