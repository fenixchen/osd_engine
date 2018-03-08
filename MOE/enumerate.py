# -*- coding:utf-8 -*-

from enum import Enum


class IngredientType(Enum):
    INVALID = 0
    RECTANGLE = 1
    LINE = 2
    GLYPH = 3
    BITMAP = 4
    LABEL = 5


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

OSD_INGREDIENT_DATA_SIZE = 5 * 4
