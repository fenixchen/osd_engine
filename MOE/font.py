# -*- coding: UTF-8 -*-

# pip.exe install freetype-py
# copy freetype.dll to current folder

import freetype
from osdobject import OSDObject
from log import Log
from enumerate import OSD_DEFAULT_FONT_SIZE

logger = Log.get_logger("engine")


class Font(OSDObject):
    BASE_DIR = ''

    def __init__(self, scene, id, file, font_width=None, font_height=None,
                 charmap=None, monochrome=False):
        super().__init__(scene, id)
        self._file = file
        self._monochrome = monochrome
        self._font_width = font_width
        self._font_height = font_height
        self._faces = []
        if isinstance(file, list):
            assert len(file) == 4
            for i, f in enumerate(file):
                face = freetype.Face(Font.BASE_DIR + file[i])
                if charmap is not None:
                    face.set_charmap(charmap)
                self._faces.append(face)
                assert self._faces[i] is not None
        else:
            face = freetype.Face(Font.BASE_DIR + file)
            self._faces.append(face)
            if charmap is not None:
                face.set_charmap(charmap)
            for i in range(3):
                self._faces.append(face)

    def load_char(self, char, font_size):
        face = self._faces[0]
        assert (face is not None)

        if font_size is None:
            font_size = self.scene.default_font_size

        if self._font_width is None:
            font_width = font_size * 64
        else:
            font_width = self._font_width * 64

        if self._font_height is None:
            font_height = font_size * 64
        else:
            font_height = self._font_height * 64

        face.set_char_size(font_width, font_height)

        if self._monochrome:
            flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO
        else:
            flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL
        face.load_char(char, flags)
        bitmap_top = (face.size.ascender >> 6) - face.glyph.bitmap_top
        # height = self._face.bbox.yMax - self._face.bbox.yMin + 1
        height = face.size.height >> 6
        left = max(0, face.glyph.bitmap_left)
        top = max(0, bitmap_top)
        width = face.glyph.advance.x >> 6;
        return left, \
               top, \
               width, \
               height, \
               self._monochrome, \
               face.glyph.bitmap

    def __str__(self):
        if isinstance(self._file, list):
            return "%s(id: %s, file:%s)" % (type(self), self.id, ",".join(self._file))
        else:
            return "%s(id: %s, file:%s)" % (type(self), self.id, self._file)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        return b'', b''
