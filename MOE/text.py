# -*- coding:utf-8 -*-


from block import Block


class Text(object):
    def __init__(self, scene, text, color, font=None, font_size=None, vertical=False):
        self._scene = scene
        self._text = text
        self._color = color
        self._font_size = font_size
        self._vertical = vertical
        self._characters = []
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

    def get_blocks(self, window, block_id, left, top):
        blocks = []
        advance = 0
        for i, character in enumerate(self._characters):
            glyph = character.glyph
            sub_id = '%s_%d' % (block_id, i)
            block = Block(window, sub_id, character, left, top)
            blocks.append(block)

            if glyph.code == ord(' '):
                if self._vertical:
                    top += advance
                else:
                    left += advance
            else:
                if self._vertical:
                    top += glyph.advance_y
                    advance = max(advance, glyph.advance_y)
                else:
                    left += glyph.advance_x
                    advance = max(advance, glyph.advance_x)
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
        return "text:%s, color:%d, font_size:%d, vertical:%d" % (
            self._text, self._color,
            self._font_size if self._font_size is not None else self._scene.default_font_size,
            self._vertical
        )
