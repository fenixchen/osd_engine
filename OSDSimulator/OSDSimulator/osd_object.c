#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "osd_object.h"


#ifndef MAX_PATH
#define MAX_PATH 256
#endif

#define LOG printf

const char *ingredient_name[] = {
    "INVALID",
    "RECTANGLE",
    "LINE",
    "GLYPH",
    "BITMAP",
};

#define MALLOC_OBJECT(type) (type*)calloc(sizeof(type), 1)
#define MALLOC_OBJECT_ARRAY(type, count) (type*)calloc(sizeof(type), count)

#define GLOBAL_FILE		"global.bin"
#define PALETTES_FILE	"palettes.bin"
#define WINDOWS_FILE	"windows.bin"
#define INGREDIENT_FILE	"ingredients.bin"
#define MODIFIERS_FILE	"modifiers.bin"
#define RAM_FILE	"ram.bin"

typedef struct _osd_binary {
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
} osd_binary;


static unsigned char *read_file(const char *base_folder,
                                const char *filename,
                                u32 *len) {
    FILE * fp;
    unsigned int length;
    unsigned char *buffer = NULL;
    char path[MAX_PATH + 1];
    sprintf(path, "%s/%s", base_folder, filename);
    fp = fopen(path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "open %s failed.\n", filename);
        return buffer;
    }
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (unsigned char *)malloc(length);
    if (fread(buffer, length, 1, fp) != 1) {
        fprintf(stderr, "read %s failed.\n", filename);
        free(buffer);
        return NULL;
    }
    *len = length;
    fclose(fp);

    LOG("%s, size[%d]\n", path, length);
    return buffer;
}

static osd_binary *osd_binary_new(const char *target_folder) {
    osd_binary *binary = MALLOC_OBJECT(osd_binary);

    binary->global = read_file(target_folder, GLOBAL_FILE, &binary->global_size);
    assert(binary->global);

    binary->palettes = read_file(target_folder, PALETTES_FILE, &binary->palettes_size);
    assert(binary->palettes);

    binary->ingredients = read_file(target_folder, INGREDIENT_FILE, &binary->ingredients_size);
    assert(binary->ingredients);

    binary->windows = read_file(target_folder, WINDOWS_FILE, &binary->windows_size);
    assert(binary->windows);

    binary->ram = read_file(target_folder, RAM_FILE, &binary->ram_size);
    assert(binary->ram);

    return binary;
}

static void osd_binary_delete(osd_binary *binary) {
    assert(binary);
    free(binary->global);
    free(binary->palettes);
    free(binary->ingredients);
    free(binary->ram);
    free(binary);
}

void log_global(osd_scene *scene) {
    LOG("global\n\twidth: %d, height:%d, ram_base_addr:%#x\n",
        scene->width, scene->height, scene->ram_base_addr);
}

void log_palette(int index, osd_palette *palette) {
    u32 i;
    LOG("palette[%d] \n\tpixel_format:%d, pixel_bits:%d, entry_count:%d, luts_addr:%#x\n",
        index,
        palette->pixel_format, palette->pixel_bits, palette->entry_count,
        (unsigned int)palette->luts_addr);
    for (i = 0; i < palette->entry_count; i ++) {
        LOG("\t%08x ", palette->lut[i]);
    }
    LOG("\n");
}

void log_ingredient(int index, osd_ingredient *ingredient) {
    osd_rectangle *rect;
    LOG("ingredient[%d] \n\ttype:%s(%d), palette:%d\n",
        index, ingredient_name[ingredient->type], ingredient->type, ingredient->palette_index);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE:
        rect = &ingredient->data.rect;
        LOG("\tgradient_mode:%d, width:%d, height:%d\n"
            "\tborder_color:[%d,%d,%d,%d], border_weight:%d, border_style:%d\n"
            "\tbgcolor:[%d,%d]\n",
            rect->gradient_mode,
            rect->width, rect->height,
            rect->border_color_top, rect->border_color_bottom, rect->border_color_left,
            rect->border_color_right, rect->border_weight, rect->border_style,
            rect->bgcolor_start, rect->bgcolor_end);
        break;
    default:
        printf("Unknown ingredient:%d\n", ingredient->type);
        assert(0);
    }
}

void log_window(int index, osd_window *window) {
    u32 i;
    LOG("window[%d] \n"
        "\tpalette:%d, visible:%d, alpha:%d, z_order:%d\n"
        "\tx:%d, y:%d, width:%d, height:%d\n"
        "\tblock_count:%d, block_addr:%#x\n",
        index,
        window->palette_index, window->visible, window->alpha, window->z_order,
        window->x, window->y, window->width, window->height,
        window->block_count, (unsigned int)window->blocks);
    for (i = 0; i < window->block_count; i ++) {
        osd_block *block = &window->blocks[i];
        printf("\t\tblock[%d] ingredient:%d, x:%d, y:%d\n",
               block->block_index, block->ingredient_index, block->x, block->y);
    }
}

osd_scene *osd_scene_new(const char *target_folder) {
    osd_binary *binary;
    osd_scene *scene;
    u32 count, i, j;
    assert(target_folder != NULL);

    binary = osd_binary_new(target_folder);
    if (!binary) {
        return NULL;
    }

    // load scene
    assert(binary->global_size == OSD_SCENE_DATA_SIZE);
    scene = MALLOC_OBJECT(osd_scene);
    memcpy(scene, binary->global, binary->global_size);
    log_global(scene);

    //load palettes
    assert(binary->palettes_size % OSD_PALETTE_DATA_SIZE == 0);
    count = binary->palettes_size / OSD_PALETTE_DATA_SIZE;
    assert(count <= OSD_SCENE_MAX_PALETE_COUNT);
    for (i = 0; i < count; i ++) {
        osd_palette *palette = MALLOC_OBJECT(osd_palette);
        memcpy(palette, binary->palettes + i * OSD_PALETTE_DATA_SIZE, OSD_PALETTE_DATA_SIZE);
        scene->palettes[i] = palette;
        if (palette->entry_count > 0) {
            u32 *lut_ram = (u32*)(binary->ram + palette->luts_addr - scene->ram_base_addr);
            palette->lut = MALLOC_OBJECT_ARRAY(u32, palette->entry_count);
            memcpy(palette->lut, lut_ram, palette->entry_count * 4);
        }
        log_palette(i, palette);
    }

    //load ingredients
    assert(binary->ingredients_size % OSD_INGREDIENT_DATA_SIZE == 0);
    assert(count <= OSD_SCENE_MAX_INGREDIENT_COUNT);
    count = binary->ingredients_size / OSD_INGREDIENT_DATA_SIZE;
    for (i = 0; i < count; i ++) {
        osd_ingredient *ingredient = MALLOC_OBJECT(osd_ingredient);
        memcpy(ingredient, binary->ingredients + i * OSD_INGREDIENT_DATA_SIZE, OSD_INGREDIENT_DATA_SIZE);
        scene->ingredients[i] = ingredient;
        log_ingredient(i, ingredient);
    }

    //load windows
    assert(binary->windows_size % OSD_WINDOW_DATA_SIZE == 0);
    count = binary->windows_size / OSD_WINDOW_DATA_SIZE;
    assert(count <= OSD_SCENE_MAX_WINDOW_COUNT);
    for (i = 0; i < count; i ++) {
        osd_window *window = MALLOC_OBJECT(osd_window);
        osd_block *block;
        memcpy(window, binary->windows + i * OSD_WINDOW_DATA_SIZE, OSD_WINDOW_DATA_SIZE);
        scene->windows[i] = window;
        window->blocks = MALLOC_OBJECT_ARRAY(osd_block, window->block_count);
        block = (osd_block *)(binary->ram + (window->blocks_addr - scene->ram_base_addr));
        for (j = 0; j < window->block_count; j ++) {
            memcpy(&window->blocks[i], block, sizeof(osd_block));
            block ++;
        }
        log_window(i, window);
    }

    osd_binary_delete(binary);
    return scene;
}

void osd_scene_delete(osd_scene *scene) {
    u32 i;
    assert(scene);

    for (i = 0; scene->palettes[i]; i ++) {
        osd_palette *palette = scene->palettes[i];
        free(palette->lut);
        free(palette);
    }

    for (i = 0; scene->ingredients[i]; i ++) {
        osd_ingredient *ingredient = scene->ingredients[i];
        free(ingredient);
        i ++;
    }

    for (i = 0; scene->windows[i]; i ++) {
        osd_window *window = scene->windows[i];
        free(window->blocks);
        free(window);
        i ++;
    }
}


static u32 osd_scene_get_color(osd_scene *scene, osd_window *window,
                               osd_ingredient *ingredient, u8 index) {
    u8 palette_index = ingredient->palette_index;
    if (palette_index == OSD_PALETTE_INDEX_INVALID) {
        palette_index = window->palette_index;
    }
    assert(palette_index != OSD_PALETTE_INDEX_INVALID);

    return scene->palettes[palette_index]->lut[index];

}
static int osd_rectangle_border_paint(osd_scene *scene, osd_window *window, osd_block *block,
                                      osd_ingredient *ingredient,
                                      u32 *window_line_buffer,
                                      u32 y) {
    u16 width, height;
    u32 x, color, margin;
    osd_rectangle *rect;
    rect = &ingredient->data.rect;
    if (rect->width == 0xFFFF) {
        width = window->width;
    } else {
        width = rect->width;
    }
    if (rect->height == 0xFFFF) {
        height = window->height;
    } else {
        height = rect->height;
    }
    if (y < rect->border_weight) {
        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_top);
        margin = rect->border_weight - (rect->border_weight - y);
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            window_line_buffer[x] = color;
        }
        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + y; x ++) {
            window_line_buffer[x] = color;
        }
        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - y; x < (u32)(block->x + width); x ++) {
            window_line_buffer[x] = color;
        }
        return 1;
    }

    if (y > (u32)(height - rect->border_weight)) {
        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_bottom);
        margin = height - y;
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            window_line_buffer[x] = color;
        }

        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + height - y; x ++) {
            window_line_buffer[x] = color;
        }

        color = osd_scene_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - height + y; x < (u32)(block->x + width); x ++) {
            window_line_buffer[x] = color;
        }
        return 1;
    }

    color = osd_scene_get_color(scene, window, ingredient, rect->border_color_left);
    for (x = block->x; x < (u32)(block->x + rect->border_weight); x ++) {
        window_line_buffer[x] = color;
    }

    color = osd_scene_get_color(scene, window, ingredient, rect->border_color_right);
    for (x = block->x + width - rect->border_weight; x < (u32)(block->x + width); x ++) {
        window_line_buffer[x] = color;
    }

    return 0;
}

static void osd_rectangle_backgroud_paint(osd_scene *scene, osd_window *window, osd_block *block,
        osd_ingredient *ingredient,
        u32 *window_line_buffer,
        u32 y) {

}

static void osd_rectangle_paint(osd_scene *scene, osd_window *window, osd_block *block,
                                osd_ingredient *ingredient,
                                u32 *window_line_buffer,
                                u32 y) {
    int is_border = 0;
    osd_rectangle *rect = &ingredient->data.rect;
    if (rect->border_weight > 0) {
        is_border = osd_rectangle_border_paint(scene, window, block, ingredient, window_line_buffer, y);
    }
    if (!is_border && rect->gradient_mode != 0) {
        osd_rectangle_backgroud_paint(scene, window, block, ingredient, window_line_buffer, y);
    }
}

static u32 osd_ingredient_start_y(osd_ingredient *ingredient) {
    assert(ingredient);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE:
        return 0;
    default:
        assert(0);
    }
    return 0;
}

static u32 osd_ingredient_height(osd_ingredient *ingredient, osd_window *window) {
    assert(ingredient);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE: {
        osd_rectangle *rect = &ingredient->data.rect;
        if (rect->height == 0xFFFF)
            return window->height;
        else
            return rect->height;
    }
    default:
        assert(0);
    }
    return 0;
}

static int osd_window_paint(osd_scene *scene, osd_window *window, u32 *window_line_buffer, u16 y) {
    u32 i;
    u16 window_y;
    int paint = 0;
    window_y = y - window->y;

    for (i = 0; i < window->block_count; i ++) {
        osd_ingredient *ingredient;
        u32 block_y_start, block_height;
        osd_block *block = &window->blocks[i];
        ingredient = scene->ingredients[block->ingredient_index];
        assert(ingredient);
        block_y_start = block->y + osd_ingredient_start_y(ingredient);
        block_height = osd_ingredient_height(ingredient, window);
        if (block_y_start <= window_y && window_y < block_y_start + block_height) {
            switch (ingredient->type) {
            case OSD_INGREDIENT_RECTANGLE:
                osd_rectangle_paint(scene, window, block, ingredient,
                                    window_line_buffer,
                                    window_y - block->y);
                ++ paint;
                break;
            default:
                assert(0);
            }
        }
    }
    return paint;
}

static u32 osd_blend_pixel(u32 dst, u32 src, u8 alpha) {
    u8 dst_r, dst_g, dst_b;
    u8 src_r, src_g, src_b;
    u8 new_r, new_g, new_b;

    if (dst == 0 || alpha == 0xFF) {
        return src;
    }
    if (alpha == 0) {
        return dst;
    }
    dst_r = OSD_R(dst);
    dst_g = OSD_G(dst);
    dst_b = OSD_B(dst);

    src_r = OSD_R(src);
    src_g = OSD_G(src);
    src_b = OSD_B(src);

    new_r = OSD_BLEND(dst_r, src_r, alpha);
    new_g = OSD_BLEND(dst_g, src_g, alpha);
    new_b = OSD_BLEND(dst_b, src_b, alpha);

    return OSD_RGB(new_r, new_g, new_b);

}

static void osd_merge_line(u32 *dst_buf, u32 *src_buf, u16 len, u32 x, u8 alpha) {
    u32 i;
    for (i = x; i < x + len; i ++) {
        dst_buf[i] = osd_blend_pixel(dst_buf[i], src_buf[i - x], alpha);
    }
}

void osd_scene_paint(osd_scene *scene, u32 frame, fn_set_pixel set_pixel, void *arg) {
    int x, y, i;
    u16 width, height;
    u32 *line_buffer, *window_line_buffer;

    assert(set_pixel && scene);

    width = scene->width;
    height = scene->height;
    line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    window_line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    for (y = 0; y < height; y ++) {
        memset(line_buffer, 0, sizeof(u32) * width);
        memset(window_line_buffer, 0, sizeof(u32) * width);
        for (i = 0; scene->windows[i]; i ++) {
            osd_window *window = scene->windows[i];
            if (!window->visible) {
                continue;
            }
            if (window->y <= y && y < window->y + window->height) {
                u16 len = OSD_MIN(scene->width - window->x, window->width);
                if (osd_window_paint(scene, window, window_line_buffer, y)) {
                    osd_merge_line(line_buffer, window_line_buffer, len, window->x, window->alpha);
                }
            }
        }
        for (x = 0; x < width; x ++) {
            if (line_buffer[x] != 0) {
                set_pixel(arg, x, y, line_buffer[x]);
            }
        }
    }
    free(window_line_buffer);
    free(line_buffer);
}