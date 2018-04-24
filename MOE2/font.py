# -*- coding: UTF-8 -*-

# pip.exe install freetype-py
# copy freetype.dll to current folder

import freetype
from log import Log
from enumerate import OSD_DEFAULT_FONT_SIZE

logger = Log.get_logger("engine")

class Font(object):
    BASE_DIR = ''

    def __init__(self, scene, id, file,
                 font_width=None, font_height=None,
                 charmap=None, monochrome=True):
        self._scene = scene
        self._id = id
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

    @property
    def id(self):
        return self._id


    def get_kerning(self, char1, char2, font_width, font_height):
        face = self._faces[0]
        assert (face is not None)
        font_width_unit = font_width * 64
        font_height_unit = font_height * 64

        face.set_char_size(font_width_unit, font_height_unit)
        kerning = face.get_kerning(char1, char2)
        return kerning.x

    def _get_bit(self, int_list, bit_offset):
        bit = int_list[bit_offset >> 3] & (128 >> (bit_offset & 7))
        return bit

    def _set_bit(self, int_list, bit_offset, value):
        byte_index = bit_offset >> 3
        bit_index = 7 - (bit_offset & 7)
        if value > 0:
            int_list[byte_index] |= 1 << bit_index
        else:
            int_list[byte_index] &= ~(1 << bit_index)

    def load_char(self, char, font_width, font_height=None):
        face = self._faces[0]
        assert face is not None
        assert font_width % 4 == 0
        if font_height is None:
            font_height = font_width
        assert font_height % 4 == 0

        font_width_unit = font_width * 64
        font_height_unit = font_height * 64
        face.set_char_size(font_width_unit, font_height_unit)

        if self._monochrome:
            flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_MONO
        else:
            flags = freetype.FT_LOAD_RENDER | freetype.FT_LOAD_TARGET_NORMAL
        face.load_char(char, flags)

        bitmap = face.glyph.bitmap
        left = max(0, face.glyph.bitmap_left)
        top = max(0, (face.size.ascender >> 6) - face.glyph.bitmap_top)
        top = min(font_height - bitmap.rows, top)
        width = face.glyph.advance.x >> 6

        glyph_data_len = font_width * font_height // 8
        glyph_data = [0] * glyph_data_len
        bitmap_buffer = bitmap.buffer
        for y in range(font_height):
            if y < top or y >= top + bitmap.rows:
                continue
            for x in range(font_width):
                if x < left or x >= left + bitmap.width:
                    continue
                src_bit_offset = bitmap.pitch * (y - top) * 8 + (x - left)
                dst_bit_offset = font_width * y + x
                bit = self._get_bit(bitmap_buffer, src_bit_offset)
                self._set_bit(glyph_data, dst_bit_offset, bit)

        return glyph_data, width

    def __str__(self):
        if isinstance(self._file, list):
            return "%s(id: %s, file:%s)" % (type(self), self.id, ",".join(self._file))
        else:
            return "%s(id: %s, file:%s)" % (type(self), self.id, self._file)

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>[%d]' % (type(self), self._id, self.object_index))
        return b'', b''


if __name__ == '__main__':
    def get_bit(int_list, bit_offset):
        bit = int_list[bit_offset >> 3] & (128 >> (bit_offset & 7))
        return bit


    def print_char(data, font_width, font_height):
        for y in range(font_height):
            for x in range(font_width):
                if get_bit(data, y * font_width + x):
                    print('*', end='')
                else:
                    print('-', end='')
            print()


    font_width = 24
    font_height = 24
    font = Font(None, '', r'scene\fonts\calibri.ttf')
    text = 'Hello world'
    previous = None
    for char in text:
        data, width = font.load_char(char, font_width, font_height)
        if previous is not None:
            kerning = font.get_kerning(previous, char, font_width, font_height)
        else:
            kerning = 0
        width_dec = font_width - width + kerning
        print("width: %d, width_dec:%d, kerning:%d" % (width, width_dec, kerning))
        print_char(data, font_width, font_height)
        previous = char
