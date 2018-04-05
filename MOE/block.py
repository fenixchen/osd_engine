# -*- coding:utf-8 -*-


class Block(object):
    def __init__(self, window, id, ingredient, x, y, visible=None, mutable=None):
        self._window = window
        self._id = id
        self._x = window.get_x(self, x)
        self._y = window.get_y(self, y)
        self._ingredient = ingredient
        self._visible = visible
        self._mutable = mutable
        self._index = 0

    @property
    def index(self):
        return self._index

    @index.setter
    def index(self, i):
        self._index = i

    @property
    def full_index(self):
        return (self._window.object_index << 16) | self.index

    @property
    def mutable(self):
        return self._mutable

    @mutable.setter
    def mutable(self, mutable):
        self._mutable = mutable

    @property
    def visible(self):
        return self._visible

    @visible.setter
    def visible(self, v):
        self._visible = v

    @property
    def id(self):
        return self._id

    @property
    def full_id(self):
        return self.window.id + '.' + self._id

    @property
    def x(self):
        return self._x

    @x.setter
    def x(self, val):
        self._x = val

    @property
    def y(self):
        return self._y

    @y.setter
    def y(self, val):
        self._y = val

    @property
    def window(self):
        return self._window

    def top_line(self):
        return self._ingredient.top_line() + self._y

    @property
    def height(self):
        return self._ingredient.height

    @property
    def width(self):
        return self._ingredient.width

    @property
    def ingredient(self):
        return self._ingredient
