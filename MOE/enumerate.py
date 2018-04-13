# -*- coding:utf-8 -*-

from enum import Enum


class FontStyle(Enum):
    NORMAL = 0
    BOLD = 1
    ITALIC = 2,
    BOLD_ITALIC = 3


class IngredientType(Enum):
    INVALID = 0
    BITMAP = 1
    TEXT = 2
    RECTANGLE = 3
    LINE = 4
    LABEL = 5

class GradientMode(Enum):
    NONE = 0
    SOLID = 1
    LEFT_TO_RIGHT = 2
    TOP_TO_BOTTOM = 3
    TOP_LEFT_TO_BOTTOM_RIGHT = 4
    BOTTOM_LEFT_TO_TOP_RIGHT = 5
    CORNER_TO_CENTER = 6


class Align(Enum):
    LEFT = 0
    CENTER = 1
    RIGHT = 2


class LineStyle(Enum):
    SOLID = 1
    DASH = 2
    DOT1 = 3
    DOT2 = 4
    DOT3 = 5
    DASH_DOT = 6
    DASH_DOT_DOT = 7


INVALID_BLOCK_INDEX = 0xFFFFFFFF

DASH_WIDTH = 10

# all size are bytes based


OSD_PALETTE_HEADER_SIZE = 2 * 4  # struct _osd_palette_hw

OSD_INGREDIENT_HEADER_SIZE = 4 * 4  # struct _osd_ingredient_hw

OSD_WINDOW_HEADER_SIZE = 8 * 4  # struct _osd_window_hw

OSD_GLYPH_HEADER_SIZE = 3 * 4  # struct _osd_glyph

OSD_SCENE_HEADER_SIZE = 8 * 4 # struct _osd_scene_hw

OSD_BLOCK_HEADER_SIZE = 3 * 4 # struct _osd_block_hw

OSD_SCENE_TITLE_MAX_LEN = 16

OSD_DEFAULT_FONT_SIZE = 16
