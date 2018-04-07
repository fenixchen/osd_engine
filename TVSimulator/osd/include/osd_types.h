#ifndef _OSD_TYPES_H
#define _OSD_TYPES_H

#include "tv_api.h"

#define OSD_SCENE_MAX_PALETE_COUNT 8
#define OSD_SCENE_MAX_INGREDIENT_COUNT 1024
#define OSD_SCENE_MAX_WINDOW_COUNT 32
#define OSD_SCENE_MAX_GLYPH_COUNT 1024

typedef struct _osd_block osd_block;
typedef struct _osd_window osd_window;
typedef struct _osd_scene osd_scene;
typedef struct _osd_palette osd_palette;
typedef struct _osd_ingredient osd_ingredient;

typedef struct _osd_rectangle osd_rectangle;
typedef struct _osd_line osd_line;

typedef struct _osd_label osd_label;

typedef struct _osd_proc osd_proc;

typedef struct _osd_character osd_character;
typedef struct _osd_glyph osd_glyph;
typedef struct _osd_bitmap osd_bitmap;

typedef struct _osd_binary osd_binary;

#define PIXEL_FORMAT_RGB  1
#define PIXEL_FORMAT_GRAY_SCALE 2
#define PIXEL_FORMAT_LUT 3

#define OSD_SCENE_HW_DATA_SIZE sizeof(osd_scene_hw)

typedef struct _osd_scene_hw osd_scene_hw;

struct _osd_scene_hw {
    u16 width, height;

    u32 ram_offset;

    u8 palette_data_size;
    u8 ingredient_data_size;
    u8 window_data_size;
    u8 glyph_header_size;

    u8 palette_count;
    u8 window_count;
    u16 ingredient_count;

    char title[12]; //3 bytes

    u16 timer_ms; //0 means no timer
    u16 glyph_count;
};

#define OSD_PALETTE_DATA_SIZE sizeof(osd_palette_hw)

typedef struct _osd_palette_hw osd_palette_hw;
struct _osd_palette_hw {
    u8 pixel_format;
    u8 pixel_bits; // 0, 1, 2, 4, 8, 16
    u16 entry_count;
    u32 luts_addr;
};

#define OSD_PALETTE_INDEX_INVALID 0xFF

#define OSD_LINE_STYLE_DASH_WIDTH 10

#define OSD_LINE_STYLE_SOLID 1
#define OSD_LINE_STYLE_DASH  2
#define OSD_LINE_STYLE_DOT1  3
#define OSD_LINE_STYLE_DOT2  4
#define OSD_LINE_STYLE_DOT3  5
#define OSD_LINE_STYLE_DASH_DOT 6
#define OSD_LINE_STYLE_DASH_DOT_DOT 7

#define OSD_GRADIENT_MODE_NONE 0
#define OSD_GRADIENT_MODE_SOLID 1
#define OSD_GRADIENT_MODE_LEFT_TO_RIGHT 2
#define OSD_GRADIENT_MODE_TOP_TO_BOTTOM 3
#define OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT 4
#define OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT 5
#define OSD_GRADIENT_MODE_CORNER_TO_CENTER 6

typedef struct _osd_rectangle_hw osd_rectangle_hw;
struct _osd_rectangle_hw {
    u16 width;
    u16 height;

    u8 border_color_top, border_color_bottom;
    u8 border_color_left, border_color_right;

    u8 border_weight;
    u8 border_style: 4; //OSD_LINE_STYLE_XXX, lower 4 bit
    u8 gradient_mode:4; //OSD_GRADIENT_MODE_TOP_TO_BOTTOM_XXX, higher 4 bit
    u8 bgcolor_start, bgcolor_end;

};

typedef struct _osd_line_hw osd_line_hw;
struct _osd_line_hw {
    u16 x1, y1;

    u16 x2, y2;

    u8 weight;
    u8 style;	//OSD_LINE_STYLE_XXX
    u8 color;
    u8 reserved2;

};

#define OSD_SCENE_INVALID_GLYPH_INDEX 0xFFFF

#define OSD_GLYPH_HEADER_SIZE sizeof(osd_glyph)

struct _osd_glyph {
    u8 left, top;
    u8 width, height;

    u16 char_code;
    u8 font_id;
    u8 font_size;

    u8 pitch;
    u8 advance_x;
    u16 data_size:15;
    u16 monochrome:1;
};

typedef struct _osd_character_hw osd_character_hw;
struct _osd_character_hw {
    u8 color;
    u8 reserved1;
    u16 reserved2;

    u32 glyph_addr; //pointer to _osd_glyph

    u32 reserved3;

};

typedef struct _osd_bitmap_data osd_bitmap_data;
struct _osd_bitmap_data {
    u16 bitmap_width;
    u16 bitmap_height;
    u32 transparent_color;
    u32 data_size;
};

typedef struct _osd_bitmap_hw osd_bitmap_hw;
struct _osd_bitmap_hw {
    u16 width;
    u16 height;

    u8 tiled: 1;
    u8 transparent: 1;
    u8 reserved: 6;
    u8 mask_color;
    u8 bitmap_count;
    u8 current_bitmap;

    u32 data_addr; //pointer to osd_bitmap_data
};


typedef struct _osd_label_state osd_label_state;
struct _osd_label_state {
    u32  color_index;
    u32 bg_block_index;
};
typedef struct _osd_label_text osd_label_text;
struct _osd_label_text {
    u32 char_block_count;
    u32 char_block_index[];
};
typedef struct _osd_label_data osd_label_data;
struct _osd_label_data {
    osd_label_state state[1]; //state_count
    /*
    osd_label_text label_text[text_count]
    */

};

typedef struct _osd_label_hw osd_label_hw;
struct _osd_label_hw {
    u16 state_count;
    u16 current_state;
    u16 text_count;
    u16 current_text;
    u32 osd_label_data_addr; //pointer to osd_label_data
};

#define OSD_INGREDIENT_RECTANGLE 1
#define OSD_INGREDIENT_LINE		 2
#define OSD_INGREDIENT_CHARACTER 3
#define OSD_INGREDIENT_BITMAP	 4
#define OSD_INGREDIENT_LABEL	 5
#define OSD_INGREDIENT_FORM		 6
#define OSD_INGREDIENT_BUTTON	 7
#define OSD_INGREDIENT_EDIT		 8

#define OSD_INGREDIENT_DATA_SIZE sizeof(osd_ingredient_hw)

typedef struct _osd_ingredient_hw osd_ingredient_hw;
struct _osd_ingredient_hw {
    u8 type; //OSD_INGREDIENT_XXX
    u8 palette_index;
    u16 flags;
    union {
        osd_rectangle_hw rectangle;
        osd_line_hw line;
        osd_bitmap_hw bitmap;
        osd_character_hw character;
        osd_label_hw label;
    } data;
};

typedef struct _osd_block_hw osd_block_hw;
struct _osd_block_hw {
    u16 visible: 1; //lowest bits
    u16 block_index: 15;
    u16 ingredient_index;
    u16 x;
    u16 y;
};


#define OSD_WINDOW_DATA_SIZE sizeof(osd_window_hw)
typedef struct _osd_window_hw osd_window_hw;

struct _osd_window_hw {
    u8 reserved;
    u8 visible;
    u8 alpha;
    u8 z_order;

    u16 x, y;

    u16 width, height;

    u32 block_count;

    u32 blocks_addr;
};

#define OSD_RGB(r, g, b) ((u32)(((u8)(r)|((u16)((u8)(g))<<8))|(((u32)(u8)(b))<<16)))
#define OSD_R(color) ((u8)(color) & 0xFF)
#define OSD_G(color) ((u8)(color >> 8) & 0xFF)
#define OSD_B(color) ((u8)(color >> 16) & 0xFF)
#define OSD_BLEND(dst, src, alpha) (u8)((dst) * (255 - (alpha)) / 255 + (src) * (alpha) / 255)

#define OSD_COLOR_CLIP(color) TV_MIN(TV_MAX(0, color), 0xFF)


#define OSD_BLOCK_INDEX_TO_WINDOW_INDEX(block_index) (((block_index) >> 16) & 0xFFFF)
#define OSD_BLOCK_INDEX_TO_WINDOW_BLOCK(block_index) ((block_index) & 0xFFFF)

#define OSD_INVALID_BLOCK_INDEX 0xFFFFFFFF

typedef struct _osd_rect osd_rect;

struct _osd_rect {
    u32 x, y;
    u32 width, height;
};

typedef struct _osd_point osd_point;
struct _osd_point {
    u32 x, y;
};

#endif

