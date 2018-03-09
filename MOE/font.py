# -*- coding: UTF-8 -*-

# pip.exe install freetype-py
# copy freetype.dll to current folder

import freetype
from osdobject import OSDObject
from log import Log

logger = Log.get_logger("engine")


class Font(OSDObject):
    BASE_DIR = ''

    def __init__(self, scene, id, file, gray_scale=True):
        super().__init__(scene, id)
        self._file = file
        self._gray_scale = gray_scale
        self._faces = []
        if isinstance(file, list):
            assert len(file) == 4
            for i, f in enumerate(file):
                self._faces.append(freetype.Face(Font.BASE_DIR + file[i]))
                assert self._faces[i] is not None
        else:
            self._faces.append(freetype.Face(Font.BASE_DIR + file))
            for i in range(3):
                self._faces.append(self._faces[0])

    def load_char(self, char, font_size):
        face = self._faces[0]
        assert (face is not None)

        if font_size is None:
            font_size = self.scene.default_font_size

        face.set_char_size(font_size * 64, font_size * 64)
        flags = 0x4
        if not self._gray_scale:
            flags |= 0x1000

        face.load_char(char, flags)
        bitmap_top = (face.size.ascender >> 6) - face.glyph.bitmap_top
        # height = self._face.bbox.yMax - self._face.bbox.yMin + 1
        height = face.size.height >> 6
        return max(0, face.glyph.bitmap_left), \
               bitmap_top, \
               face.glyph.advance.x >> 6, \
               height, \
               face.glyph.bitmap

    def __str__(self):
        if isinstance(self._file, list):
            return "%s(id: %s, file:%s)" % (type(self), self.id, ",".join(self._file))
        else:
            return "%s(id: %s, file:%s)" % (type(self), self.id, self._file)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        return b'', b''
