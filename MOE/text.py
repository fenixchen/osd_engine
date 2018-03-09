# -*- coding:utf-8 -*-


from block import Block


class Text(object):
    def __init__(self, scene, text, color, font=None, font_size=None, vertical=False):
        self._scene = scene
        self._text = text
        self._color = color
        self._font_size = font_size
        self._vertical = vertical
        self._glyphs = []
        for char in self._text:
            glyph = scene.get_glyph(char, font, font_size)
            assert glyph is not None
            glyph.color = color
            self._glyphs.append(glyph)

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        i = 0
        for glyph in self._glyphs:
            sub_id = '%s_%d' % (block_id, i)
            block = Block(window, sub_id, glyph, left, top)
            blocks.append(block)
            if self._vertical:
                top += glyph.advance_y
            else:
                left += glyph.advance_x
            i += 1
        return blocks

    @property
    def width(self):
        w = 0
        for glyph in self._glyphs:
            w += glyph.advance_x
        return w

    @property
    def height(self):
        h = 0
        for glyph in self._glyphs:
            h = max(h, glyph.advance_y)
        return h

    def __str__(self):
        return "text:%s, color:%d, font_size:%d, vertical:%d" % (
            self._text, self._color,
            self._font_size if self._font_size is not None else self._scene.default_font_size,
            self._vertical
        )
