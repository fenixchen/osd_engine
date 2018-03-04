# -*- coding:utf-8 -*-

import abc

from osdobject import OSDObject


class Ingredient(OSDObject):

    def __init__(self, scene, id, palette):
        self._scene = scene
        self._id = id
        if palette is None:
            self._palette = None
        else:
            self._palette = scene.find_palette(palette)

    @property
    def id(self):
        return self._id

    @property
    def scene(self):
        return self._scene

    @property
    def palette(self):
        return self._palette

    def color(self, window, color_index):
        if self._palette is None:
            return window.palette.color(color_index)
        else:
            return self._palette.color(color_index)

    def start_y(self):
        """
        返回需要画的起始行
        :return:
        """
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
