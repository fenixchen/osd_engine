# -*- coding:utf-8 -*-


from block import Block
from enumerate import *


class Text(object):
    def __init__(self, scene, text, color,
                 font=None, font_size=None,
                 width=None,
                 align=Align.LEFT.name):
        self._scene = scene
        self._text = text
        self._color = color
        self._font_size = font_size
        self._width = width
        self._characters = []
        self._align = Align[align]
        if isinstance(self._text, str):
            for char in self._text:
                character = scene.get_character(char, color, font, font_size)
                assert character is not None
                self._characters.append(character)

        elif isinstance(self._text, int):
            character = scene.get_character(self._text, color, font, font_size)
            assert character is not None
            self._characters.append(character)
        else:
            raise Exception('unknown type <%s>' % type(self._text))

    def get_blocks(self, window, block_id, left, top, visible=None):
        blocks = []
        left_origin = left
        text_width = self.width

        if self._width is not None and self._width > text_width:
            if self._align == Align.CENTER:
                left += int((self._width - text_width) / 2)
            elif self._align == Align.RIGHT:
                left += int(self._width - text_width)

        for i, character in enumerate(self._characters):
            glyph = character.glyph
            sub_id = '%s_%d' % (block_id, i)
            block = Block(window, sub_id, character, left, top, visible)
            blocks.append(block)
            left += glyph.advance_x

            if self._width is not None and left - left_origin > self._width:
                break

        return blocks

    @property
    def width(self):
        w = 0
        for character in self._characters:
            glyph = character.glyph
            w += glyph.advance_x
        return w

    @property
    def height(self):
        h = 0
        for character in self._characters:
            glyph = character.glyph
            h = max(h, glyph.advance_y)
        return h

    def __str__(self):
        return "text:%s, color:%d, font_size:%d, width:%d" % (
            self._text, self._color,
            self._font_size if self._font_size is not None else self._scene.default_font_size,
            0 if self._width is None else self._width
        )
