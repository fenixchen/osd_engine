# -*- coding: UTF-8 -*-

# pip.exe install freetype-py
# copy freetype.dll to current folder

import freetype


class Font(object):
    BASE_DIR = ''

    def __init__(self, gray_scale=True):
        self._face = None
        self._gray_scale = gray_scale

    def load_char(self, ch, width):
        if self._face is None:
            self._face = freetype.Face(Font.BASE_DIR + "Fonts/Vera.ttf")
        assert (self._face is not None)
        self._face.set_char_size(0, width * 64)
        flags = 0x4
        if not self._gray_scale:
            flags |= 0x1000
        self._face.load_char(ch, flags)
        bitmap_top = (self._face.size.ascender >> 6) - self._face.glyph.bitmap_top
        height = self._face.bbox.yMax - self._face.bbox.yMin
        return self._face.glyph.bitmap_left, \
               bitmap_top, \
               self._face.glyph.advance.x >> 6, \
               height >> 6, \
               self._face.glyph.bitmap


if __name__ == '__main__':
    Font.BASE_DIR = 'scene/'
    font = Font(False)
    top, left, bitmap = font.load_char('S', 12)
    print("left:%d, top:%d" % (top, left))
    print("rows:%d, width:%d, pitch:%d" % (bitmap.rows, bitmap.width, bitmap.pitch))
    for y in range(bitmap.rows):
        for x in range(bitmap.pitch):
            pos = bitmap.pitch * y + x
            print("%02x " % bitmap.buffer[pos], end='')
        print()
    for y in range(bitmap.rows):
        for x in range(bitmap.width):
            pos = y * bitmap.pitch + (bitmap.width >> 3)
            if bitmap.buffer[pos] & (128 >> (x & 7)):
                print('*', end='')
            else:
                print('-', end='')
        print()
