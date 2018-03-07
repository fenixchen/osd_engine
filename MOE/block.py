# -*- coding:utf-8 -*-

class Block(object):
    def __init__(self, window, id, ingredient, x, y):
        self._window = window
        self._id = id
        self._x = x
        self._y = y
        self._ingredient = ingredient

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

    def height(self, window):
        return self._ingredient.height(window)

    @property
    def ingredient(self):
        return self._ingredient