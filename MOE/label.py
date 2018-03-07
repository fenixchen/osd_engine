# -*- coding:utf-8 -*-


from ingredient import Ingredient
from log import Log

logger = Log.get_logger("engine")


class Label(Ingredient):
    def __init__(self, scene, id, text, color, font_width, palette=None):
        super().__init__(scene, id, palette)
        self._text = text
        self._color = color
        self._font_width = font_width

    @property
    def top_line(self):
        return 0

    def height(self, window):
        return 0

    def draw_line(self, line_buf, window, y, block_x):
        pass

    def to_binary(self, ram_offset):
        logger.debug('Generate %s <%s>' % (type(self), self._id))
        return b'', b''

    def __str__(self):
        return "%s(id:%s, text:%s, color:%d, font_width:%d, palette:%s)" % (
            type(self),
            self._id, self._text, self._color, self._font_width,
            self._palette.id if self._palette is not None else 'None'
        )
