# -*- coding: UTF-8 -*-

# pip.exe install freetype-py
# copy freetype.dll to current folder

import freetype
from osdobject import OSDObject
from log import Log
from enumerate import OSD_DEFAULT_FONT_SIZE

logger = Log.get_logger("engine")


class Preload(OSDObject):

    def __init__(self, scene, id, font, size, text):
        super().__init__(scene, id)
        self._font = scene.find_font(font)
        self._text = text
        self._size = size
        for char in text:
            scene.get_glyph(char, self._font, size)

    def __str__(self):
        return "%s(id: %s, font:%s, size:%d, text:%s)" % (type(self), self.id, self._font.id, self._size, self._text)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        return b'', b''
