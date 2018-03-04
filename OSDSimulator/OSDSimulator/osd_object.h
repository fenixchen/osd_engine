#ifndef _OSD_OBJECT_H
#define _OSD_OBJECT_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#define OSD_SCENE_MAX_PALETE_COUNT 8
#define OSD_SCENE_MAX_INGREDIENT_COUNT 256
#define OSD_SCENE_MAX_WINDOW_COUNT 32
#define OSD_WINDOW_MAX_BLOCK_COUNT 32
#define OSD_MODIFIER_MAX_WINDOW_COUNT 4
#define OSD_MODIFIER_MAX_BLOCK_COUNT 4


typedef struct _osd_block osd_block;
typedef struct _osd_window osd_window;
typedef struct _osd_scene osd_scene;
typedef struct _osd_palette osd_palette;
typedef struct _osd_ingredient osd_ingredient;
typedef struct _osd_modifier osd_modifier;

typedef struct _osd_rectangle osd_rectangle;
typedef struct _osd_line osd_line;
typedef struct _osd_move osd_move;
typedef struct _osd_flip osd_flip;

typedef struct _osd_glyph osd_glyph;
typedef struct _osd_bitmap osd_bitmap;

const u8 PIXEL_FORMAT_RGB = 1;
const u8 PIXEL_FORMAT_GRAY_SCALE = 2;
const u8 PIXEL_FORMAT_LUT = 3;

struct _osd_scene {
    u16 width, height;
    osd_palette *palettes[OSD_SCENE_MAX_PALETE_COUNT];
    osd_ingredient *ingredients[OSD_SCENE_MAX_INGREDIENT_COUNT];
    osd_window *window[OSD_SCENE_MAX_WINDOW_COUNT];
};

struct _osd_palette {
    u8 pixel_format;
    u8 pixel_bits; // 1, 2, 4, 8, 16
    u16 entry_count;
    u8 *luts;
};

const u8 OSD_LINE_STYLE_SOLID = 1;
const u8 OSD_LINE_STYLE_DASH = 2;
const u8 OSD_LINE_STYLE_DOT1 = 3;
const u8 OSD_LINE_STYLE_DOT2 = 4;
const u8 OSD_LINE_STYLE_DOT3 = 5;
const u8 OSD_LINE_STYLE_DASH_DOT = 6;
const u8 OSD_LINE_STYLE_DASH_DOT_DOT = 7;

const u8 OSD_GRADIENT_MODE_TOP_TO_BOTTOM = 1;
const u8 OSD_GRADIENT_MODE_BOTTOM_TO_TOP = 2;
const u8 OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT = 3;
const u8 OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT = 4;

struct _osd_rectangle {
    u8 gradient_mode; //OSD_GRADIENT_MODE_TOP_TO_BOTTOM_XXX
    u8 reserved;

    u16 width;
    u16 height;

    u8 border_color_top, border_color_bottom, border_color_left, border_color_right;

    u8 border_weight;
    u8 border_style; //OSD_LINE_STYLE_XXX
    u8 bgcolor_start, bgcolor_end;

};

struct _osd_line {
    u16 reserved1;
    u16 x1, y1, x2, y2;

    u8 weight;
    u8 style;	//OSD_LINE_STYLE_XXX
    u8 color;
    u8 reserved2;
};

struct _osd_glyph {
    u16 reserved1;

    u8 bitmap_left, bitmap_top, bitmap_width, bitmap_height;

    u8 bitmap_pitch;
    u8 color;
    u8 font_width;
    u8 reserved;

    u16 char_code;
    u16 data_size;

    u8 *data; //data_size
};

struct _osd_bitmap {
    u16 reserved1;

    u16 width, height;

    u8 bitmap_count;
    u8 reserved;
    u16 data_size;

    u8 *data; //data_size
};

const u8 OSD_INGREDIENT_RECTANGLE = 1;
const u8 OSD_INGREDIENT_LINE = 2;
const u8 OSD_INGREDIENT_GLYPH = 3;
const u8 OSD_INGREDIENT_BITMAP = 4;

struct _osd_ingredient {
    u8 type; //OSD_INGREDIENT_XXX
    u8 palette_index;
    u16 reserved;
    union {
        osd_rectangle rect;
        osd_line line;
        osd_glyph glyph;
        osd_bitmap bitmap;
    } data;
};

struct _osd_move {
    u8 x_delta, y_delta;
    u8 w_delta, h_delta;
};

struct _osd_flip {
    u8 loop;
};

const u8 OSD_MODIFIER_MOVE = 1;
const u8 OSD_MODIFIER_FLIP = 2;
struct _osd_modifier {
    u8 type;
    u8 interval;
    u8 limit;
    u8 active;
    u8 *windows[OSD_MODIFIER_MAX_WINDOW_COUNT];
    u8 *blocks[OSD_MODIFIER_MAX_BLOCK_COUNT];
    union {
        osd_move move;
        osd_flip flip;
    } data;
};

struct _osd_block {
    u16 block_index;
    u16 ingredient_index;
    u16 x;
    u16 y;
};

struct _osd_window {
    u8 palette_index;
    u8 visible;
    u8 alpha;
    u8 z_order;
    u16 x, y;
    u16 width, height;
    osd_block *blocks[OSD_WINDOW_MAX_BLOCK_COUNT];
};


EXTERNC osd_scene *osd_scene_new(const char *target_folder);

EXTERNC void osd_scene_delete(osd_scene *scene);


#endif