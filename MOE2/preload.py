# -*- coding: UTF-8 -*-

from log import Log

logger = Log.get_logger("engine")


class Preload(object):

    def __init__(self, window, id, font, size, text):
        self._font = window.scene.find_font(font)
        self._text = text
        self._size = size
        for char in text:
            window.get_glyph(char, self._font, size)

    def __str__(self):
        return "%s(id: %s, font:%s, size:%d, text:%s)" % (
            type(self), self.id, self._font.id, self._size, self._text)
