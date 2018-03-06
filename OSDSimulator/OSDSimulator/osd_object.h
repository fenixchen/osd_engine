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


#define OSD_OFFSET_OF(type, member)   (size_t)&(((type *)0)->member)

#define OSD_SCENE_MAX_PALETE_COUNT 8
#define OSD_SCENE_MAX_INGREDIENT_COUNT 256
#define OSD_SCENE_MAX_WINDOW_COUNT 32

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

typedef struct _osd_binary osd_binary;

const u8 PIXEL_FORMAT_RGB = 1;
const u8 PIXEL_FORMAT_GRAY_SCALE = 2;
const u8 PIXEL_FORMAT_LUT = 3;

#define OSD_SCENE_DATA_SIZE OSD_OFFSET_OF(osd_scene, palettes)

struct _osd_scene {
    u16 width, height;
    u32 ram_base_addr;
    osd_palette *palettes[OSD_SCENE_MAX_PALETE_COUNT];
    osd_ingredient *ingredients[OSD_SCENE_MAX_INGREDIENT_COUNT];
    osd_window *windows[OSD_SCENE_MAX_WINDOW_COUNT];
    osd_binary *binary;
};

struct _osd_binary {
    u8 *global;
    u32 global_size;

    u8 *palettes;
    u32 palettes_size;

    u8 *ingredients;
    u32 ingredients_size;

    u8 *windows;
    u32 windows_size;

    u8 *ram;
    u32 ram_size;
};

#define OSD_PALETTE_DATA_SIZE OSD_OFFSET_OF(osd_palette, lut)

struct _osd_palette {
    u8 pixel_format;
    u8 pixel_bits; // 0, 1, 2, 4, 8, 16
    u16 entry_count;
    u32 luts_addr;

    u32 *lut;
};

const u8 OSD_PALETTE_INDEX_INVALID = 0xFF;

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

struct _osd_rectangle {
    u16 width;
    u16 height;

    u8 border_color_top, border_color_bottom;
    u8 border_color_left, border_color_right;

    u8 border_weight;
    u8 border_style: 4; //OSD_LINE_STYLE_XXX, lower 4 bit
    u8 gradient_mode:4; //OSD_GRADIENT_MODE_TOP_TO_BOTTOM_XXX, higher 4 bit
    u8 bgcolor_start, bgcolor_end;
};

struct _osd_line {
    u16 x1, y1;

    u16 x2, y2;

    u8 weight;
    u8 style;	//OSD_LINE_STYLE_XXX
    u8 color;
    u8 reserved2;
};

struct _osd_glyph {
    u8 bitmap_left, bitmap_top;

    u8 bitmap_width, bitmap_height;
    u8 bitmap_pitch;
    u8 color;

    u8 font_width;
    u8 reserved;

    u16 char_code;
    u16 data_size;
};

struct _osd_bitmap {
    u16 width;
    u16 height;

    u8 bitmap_count;
    u8 reserved2;
    u16 data_size;

    u32 data_addr;
};

#define OSD_INGREDIENT_RECTANGLE 1
#define OSD_INGREDIENT_LINE		 2
#define OSD_INGREDIENT_GLYPH	 3
#define OSD_INGREDIENT_BITMAP	 4

#define OSD_INGREDIENT_DATA_SIZE OSD_OFFSET_OF(osd_ingredient, ram_data)

struct _osd_ingredient {
    u8 type; //OSD_INGREDIENT_XXX
    u8 palette_index;
    u8 current_bitmap;
    u8 reserved;

    union {
        osd_rectangle rect;
        osd_line line;
        osd_bitmap bitmap;
        //osd_glyph glyph;
    } data;

    u8 *ram_data;
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
    u16 windows_count;
    u16 blocks_count;
    u8 *windows;
    u8 *blocks;
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


#define OSD_WINDOW_DATA_SIZE OSD_OFFSET_OF(osd_window, blocks)

struct _osd_window {
    u8 palette_index;
    u8 visible;
    u8 alpha;
    u8 z_order;

    u16 x, y;
    u16 width, height;

    u32 block_count;
    u32 blocks_addr;

    osd_block *blocks;
};



#define OSD_RGB(r, g, b) ((u32)(((u8)(r)|((u16)((u8)(g))<<8))|(((u32)(u8)(b))<<16)))
#define OSD_R(color) ((u8)(color) & 0xFF)
#define OSD_G(color) ((u8)(color >> 8) & 0xFF)
#define OSD_B(color) ((u8)(color >> 16) & 0xFF)
#define OSD_BLEND(dst, src, alpha) (u8)((dst) * (255 - (alpha)) / 255 + (src_r) * (alpha) / 255)


#define OSD_MAX(x, y) ((x) > (y) ? (x) : (y))
#define OSD_MIN(x, y) ((x) < (y) ? (x) : (y))

#define OSD_COLOR_CLIP(color) OSD_MIN(OSD_MAX(0, color), 0xFF)

EXTERNC osd_scene *osd_scene_new(const char *target_folder);

EXTERNC void osd_scene_delete(osd_scene *scene);

typedef void (*fn_set_pixel)(void *arg, int x, int y, u32 color);

EXTERNC void osd_scene_paint(osd_scene *scene, u32 frame,
                             fn_set_pixel set_pixel, void *arg);

#endif

