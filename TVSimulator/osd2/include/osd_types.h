#ifndef _OSD_TYPES_H
#define _OSD_TYPES_H

#include "tv_api.h"

typedef struct _osd_window osd_window;
typedef struct _osd_scene osd_scene;
typedef struct _osd_palette osd_palette;
typedef struct _osd_glyph osd_glyph;

typedef struct _osd_rectangle osd_rectangle;

typedef struct _osd_label osd_label;

typedef struct _osd_proc osd_proc;

typedef struct _osd_text osd_text;
typedef struct _osd_bitmap osd_bitmap;

typedef struct _osd_binary osd_binary;

typedef struct _tv_app tv_app;

#define OSD_SCENE_HEADER_SIZE sizeof(osd_scene_hw)

typedef struct _osd_scene_hw osd_scene_hw;

struct _osd_scene_hw {
    u16 width, height;

    u32 target_address;

    u8 palette_header_size;    
    u8 window_header_size;
    u8 rectangle_size;
    u8 cell_size;
    
    u8 window_count;
    u8 reserved1;
    u16 timer_ms; //0 means no timer

    char title[16]; //4 words
};

#define OSD_CELL_SIZE size(osd_cell_hw)
typedef struct _osd_cell_hw osd_cell_hw;
#progam pack(1)
struct _osd_cell_hw
{
    u8 index;
    u8 color;
    u8 width_dec;
};
#progam pack()

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
    u16 x, y;
    
    u16 width, height;
    
    u8 border_color_top, border_color_bottom;
    u8 border_color_left, border_color_right;

    u8 border_weight: 7;
	u8 visible: 1;
    u8 border_style: 4; //OSD_LINE_STYLE_XXX, lower 4 bit
    u8 gradient_mode:4; //OSD_GRADIENT_MODE_TOP_TO_BOTTOM_XXX, higher 4 bit
    u8 bgcolor_start, bgcolor_end;    
};


#define OSD_WINDOW_HEADER_SIZE sizeof(osd_window_hw)
typedef struct _osd_window_hw osd_window_hw;

struct _osd_window_hw {
    u8 visible: 1;
	u8 reserved: 7;
    u8 alpha;
    u8 z_order;
    u8 reserved;

    u16 x, y;

    u16 width, height;

    u16 rectangle_count;    
    u16 row_count;
    
    u32 rectangle_addr;     //==> osd_rectangle_hw[rectangle_count]    
    
    u16 palette0_entry_count;
    u16 palette1_entry_count;
    
    u32 palette0_addr;		//==> osd_palette_hw    
    u32 palette1_addr;		//==> osd_palette_hw

    u32 rows_addr;		//==> osd_row_hw[row_count]

};

typedef struct _osd_row_hw osd_row_hw;
struct _osd_row_hw {
	u16 x, y;
    u32 resource_addr: 32;
	u8 is_glyph_row:1;
	u8 visible:1;
	u8 reserved1: 6;
    u8 cell_width;
    u8 cell_height;
	u8 reserved2;	
    osd_cell_hw cell[1];
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

#define OSD_INVALID_GLYPH_INDEX 0xFFFF

typedef struct _osd_rect osd_rect;

struct _osd_rect {
    s32 x, y;
    u32 width, height;
};

typedef struct _osd_point osd_point;
struct _osd_point {
    s32 x, y;
};

#define SHOW_OSD_OBJECT_DUMP 0

#if SHOW_OSD_OBJECT_DUMP
#define OSD_OBJECT_DUMP(object) object->dump(object)
#else
#define OSD_OBJECT_DUMP(object)
#endif


#endif

