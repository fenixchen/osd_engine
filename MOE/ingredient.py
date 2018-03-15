# -*- coding:utf-8 -*-

import abc

from block import Block
from osdobject import OSDObject


class Ingredient(OSDObject):

    def __init__(self, scene, id, palette, mutable = False):
        super().__init__(scene, id)
        if palette is None:
            self._palette = None
        else:
            self._palette = scene.find_palette(palette)
        self._mutable = mutable

    @property
    def multable(self):
        return self._mutable

    @property
    def palette(self):
        return self._palette

    def palette_index(self):
        if self._palette is None:
            return 0xFF
        else:
            return self._palette.object_index

    def get_color(self, window, color_index):
        if self._palette is None:
            return window.palette.color(color_index)
        else:
            return self._palette.color(color_index)

    def get_blocks(self, window, block_id, left, top):
        block = Block(window, block_id, self, left, top)
        return block,

    def top_line(self):
        return 0

    @abc.abstractmethod
    def height(self, window):
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
