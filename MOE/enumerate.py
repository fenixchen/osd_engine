# -*- coding:utf-8 -*-

from enum import Enum


class FontStyle(Enum):
    NORMAL = 0
    BOLD = 1
    ITALIC = 2,
    BOLD_ITALIC = 3


class IngredientType(Enum):
    INVALID = 0
    RECTANGLE = 1
    LINE = 2
    CHARACTER = 3
    BITMAP = 4
    LABEL = 5
    FORM = 6
    BUTTON = 7
    EDIT = 8


class PixelFormat(Enum):
    INVALID = 0
    RGB = 1
    GRAY_SCALE = 2
    LUT = 3

    def __int__(self):
        return self.value


class GradientMode(Enum):
    NONE = 0
    SOLID = 1
    LEFT_TO_RIGHT = 2
    TOP_TO_BOTTOM = 3
    TOP_LEFT_TO_BOTTOM_RIGHT = 4
    BOTTOM_LEFT_TO_TOP_RIGHT = 5
    CORNER_TO_CENTER = 6

    def __int__(self):
        return self.value


class LineStyle(Enum):
    SOLID = 1
    DASH = 2
    DOT1 = 3
    DOT2 = 4
    DOT3 = 5
    DASH_DOT = 6
    DASH_DOT_DOT = 7

    def __int__(self):
        return self.value


DASH_WIDTH = 10

# all size are bytes based


OSD_PALETTE_DATA_SIZE = 2 * 4  # struct _osd_palette

OSD_INGREDIENT_DATA_SIZE = 4 * 4  # struct _osd_ingredient

OSD_WINDOW_DATA_SIZE = 5 * 4  # struct _osd_window

OSD_GLYPH_HEADER_SIZE = 3 * 4  # struct _osd_scene

OSD_SCENE_TITLE_MAX_LEN = 12

OSD_SCENE_HEADER_SIZE = 8 * 4


OSD_DEFAULT_FONT_SIZE = 16