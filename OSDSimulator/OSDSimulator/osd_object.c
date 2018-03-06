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
        printf("open <%s> failed.\n", path);
        return buffer;
    }
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (unsigned char *)malloc(length);
    if (fread(buffer, length, 1, fp) != 1) {
        printf("read <%s> failed.\n", path);
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
//    u32 i;
    LOG("palette[%d] \n\tpixel_format:%d, pixel_bits:%d, entry_count:%d, luts_addr:%#x\n",
        index,
        palette->pixel_format, palette->pixel_bits, palette->entry_count,
        (unsigned int)palette->luts_addr);
#if 0
    for (i = 0; i < palette->entry_count; i ++) {
        LOG("\t%08x ", palette->lut[i]);
    }
    LOG("\n");
#endif
}

void log_ingredient(int index, osd_ingredient *ingredient) {
    LOG("ingredient[%d] \n\ttype:%s(%d), palette:%d\n",
        index, ingredient_name[ingredient->type], ingredient->type, ingredient->palette_index);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE: {
        osd_rectangle *rect = &ingredient->data.rect;
        LOG("\tgradient_mode:%d, width:%d, height:%d\n"
            "\tborder_color:[%d,%d,%d,%d], border_weight:%d, border_style:%d\n"
            "\tbgcolor:[%d,%d]\n",
            rect->gradient_mode,
            rect->width, rect->height,
            rect->border_color_top, rect->border_color_bottom, rect->border_color_left,
            rect->border_color_right, rect->border_weight, rect->border_style,
            rect->bgcolor_start, rect->bgcolor_end);
        break;
    }
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        LOG("\tx1:%d, y1:%d, x2:%d, y2:%d\n", line->x1, line->y1, line->x2, line->y2);
        break;
    }
    case OSD_INGREDIENT_BITMAP: {
        osd_bitmap *bitmap = &ingredient->data.bitmap;
        LOG("\twidth:%d, height:%d, count:%d, size:%d, addr:%#x\n",
            bitmap->width, bitmap->height, bitmap->bitmap_count,
            bitmap->data_size, bitmap->data_addr);
        break;
    }
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
    osd_scene *scene;
    u32 count, i, j;
    osd_binary *binary;
    assert(target_folder != NULL);

    scene = MALLOC_OBJECT(osd_scene);

    LOG("OSD_INGREDIENT_DATA_SIZE:%d", OSD_INGREDIENT_DATA_SIZE);
    assert(OSD_INGREDIENT_DATA_SIZE == 4 * sizeof(u32));

    binary = osd_binary_new(target_folder);
    if (!binary) {
        free(scene);
        return NULL;
    }
    scene->binary = binary;

    // load scene
    assert(binary->global_size == OSD_SCENE_DATA_SIZE);

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
            palette->lut = lut_ram;
        }
        log_palette(i, palette);
    }

    //load ingredients
    assert(binary->ingredients_size % OSD_INGREDIENT_DATA_SIZE == 0);
    assert(count <= OSD_SCENE_MAX_INGREDIENT_COUNT);
    count = binary->ingredients_size / OSD_INGREDIENT_DATA_SIZE;
    for (i = 0; i < count; i ++) {
        osd_ingredient *ingredient = MALLOC_OBJECT(osd_ingredient);
        memcpy(ingredient,
               binary->ingredients + i * OSD_INGREDIENT_DATA_SIZE,
               OSD_INGREDIENT_DATA_SIZE);
        scene->ingredients[i] = ingredient;
        if (ingredient->type == OSD_INGREDIENT_BITMAP) {
            osd_bitmap *bitmap = &ingredient->data.bitmap;
            ingredient->ram_data = (u8*)(binary->ram + bitmap->data_addr - scene->ram_base_addr);
        }
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
            memcpy(&window->blocks[j], block, sizeof(osd_block));
            block ++;
        }
        log_window(i, window);
    }

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

    free(scene->binary);
}


static int osd_line_style_check(u32 style, u32 x) {
    switch (style) {
    case OSD_LINE_STYLE_SOLID:
        return 1;
    case OSD_LINE_STYLE_DASH:
        return (x % (OSD_LINE_STYLE_DASH_WIDTH + 1)) < OSD_LINE_STYLE_DASH_WIDTH;
    case OSD_LINE_STYLE_DOT1:
        return (x % 2) == 0;
    case OSD_LINE_STYLE_DOT2:
        return (x % 3) == 0;
    case OSD_LINE_STYLE_DOT3:
        return (x % 4) == 0;
    case OSD_LINE_STYLE_DASH_DOT: {
        u32 index = x % (OSD_LINE_STYLE_DASH_WIDTH + 3);
        return index < OSD_LINE_STYLE_DASH_WIDTH || index  == OSD_LINE_STYLE_DASH_WIDTH + 1;
    }
    case OSD_LINE_STYLE_DASH_DOT_DOT: {
        u32 index = x % (OSD_LINE_STYLE_DASH_WIDTH + 5);
        return index < OSD_LINE_STYLE_DASH_WIDTH
               || index  == OSD_LINE_STYLE_DASH_WIDTH + 1
               || index  == OSD_LINE_STYLE_DASH_WIDTH + 3;
    }
    default:
        assert(0);
    }
    return 0;
}


static u32 osd_ingredient_get_color(osd_scene *scene, osd_window *window,
                                    osd_ingredient *ingredient, u32 index) {
    osd_palette *palette;
    u8 palette_index = ingredient->palette_index;
    if (palette_index == OSD_PALETTE_INDEX_INVALID) {
        palette_index = window->palette_index;
    }
    assert(palette_index != OSD_PALETTE_INDEX_INVALID);
    palette = scene->palettes[palette_index];
    assert(palette);
    assert(index < palette->entry_count);
    return palette->lut[index];
}

static int osd_rectangle_border_paint(osd_scene *scene, osd_window *window, osd_block *block,
                                      osd_ingredient *ingredient,
                                      u32 *window_line_buffer,
                                      u32 y) {
    u32 x, color, margin;
    osd_rectangle *rect = &ingredient->data.rect;
    u16 width = rect->width == 0xFFFF ? window->width : rect->width;
    u16 height = rect->height == 0xFFFF ? window->height : rect->height;
    if (y < rect->border_weight) {
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_top);
        margin = rect->border_weight - (rect->border_weight - y);
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            window_line_buffer[x] = color;
        }
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + y; x ++) {
            window_line_buffer[x] = color;
        }
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - y; x < (u32)(block->x + width); x ++) {
            window_line_buffer[x] = color;
        }
        return 1;
    }

    if (y > (u32)(height - rect->border_weight)) {
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_bottom);
        margin = height - y;
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            window_line_buffer[x] = color;
        }

        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + height - y; x ++) {
            window_line_buffer[x] = color;
        }

        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - height + y; x < (u32)(block->x + width); x ++) {
            window_line_buffer[x] = color;
        }
        return 1;
    }

    color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
    for (x = block->x; x < (u32)(block->x + rect->border_weight); x ++) {
        window_line_buffer[x] = color;
    }

    color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
    for (x = block->x + width - rect->border_weight; x < (u32)(block->x + width); x ++) {
        window_line_buffer[x] = color;
    }

    return 0;
}

static u32 OSD_COLOR_ADD(u32 color, u8 r_delta, u8 g_delta, u8 b_delta) {
    u8 r = OSD_COLOR_CLIP(OSD_R(color) + r_delta);
    u8 g = OSD_COLOR_CLIP(OSD_G(color) + g_delta);
    u8 b = OSD_COLOR_CLIP(OSD_B(color) + b_delta);
    return OSD_RGB(r, g, b);
}

static void osd_rectangle_backgroud_paint(osd_scene *scene, osd_window *window, osd_block *block,
        osd_ingredient *ingredient,
        u32 *window_line_buffer,
        u32 y) {
    u32 x, bg_color_start, bg_color_end, color, color_steps;
    double r_delta, g_delta, b_delta;

    osd_rectangle *rect = &ingredient->data.rect;
    u16 width = rect->width == 0xFFFF ? window->width : rect->width;
    u16 height = rect->height == 0xFFFF ? window->height : rect->height;

    bg_color_start = osd_ingredient_get_color(scene, window, ingredient, rect->bgcolor_start);
    bg_color_end = osd_ingredient_get_color(scene, window, ingredient, rect->bgcolor_end);

    switch (rect->gradient_mode) {
    case OSD_GRADIENT_MODE_NONE:
        return;
    case OSD_GRADIENT_MODE_SOLID:
        color_steps = 1;
        break;
    case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
        color_steps = width - rect->border_weight * 2;
        break;
    case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
        color_steps = height - rect->border_weight * 2;
        break;
    case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
    case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
        color_steps = (width - rect->border_weight * 2) * height - rect->border_weight * 2;
        break;
    default:
        assert(0);
    }
    r_delta = (OSD_R(bg_color_end) - OSD_R(bg_color_start)) / (double)color_steps;
    g_delta = (OSD_G(bg_color_end) - OSD_G(bg_color_start)) / (double)color_steps;
    b_delta = (OSD_B(bg_color_end) - OSD_B(bg_color_start)) / (double)color_steps;

    color = bg_color_start;
    for (x = block->x + rect->border_weight;
            x < (u32)(block->x + width - rect->border_weight); ++ x) {
        u32 factor;
        switch (rect->gradient_mode) {
        case OSD_GRADIENT_MODE_SOLID:
            factor = 0;
            break;
        case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
            factor = x - (block->x + rect->border_weight);
            break;
        case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
            factor = y;
            break;
        case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
            factor = (x - (block->x + rect->border_weight)) * y;
            break;
        case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
            factor = (x - (block->x + rect->border_weight)) * (rect->height - rect->border_weight - y);
            break;
        default:
            assert(0);
        }
        color = OSD_COLOR_ADD(bg_color_start,
                              (u8)(r_delta * factor),
                              (u8)(g_delta * factor),
                              (u8)(b_delta * factor));
        window_line_buffer[x] = color;
    }
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

static void osd_line_paint(osd_scene *scene, osd_window *window, osd_block *block,
                           osd_ingredient *ingredient,
                           u32 *window_line_buffer,
                           u32 y) {
    u32 x;
    osd_line *line = &ingredient->data.line;
    u32 color = osd_ingredient_get_color(scene, window, ingredient, line->color);
    u32 x1 = line->x1;
    u32 x2 = line->x2;
    u32 y1 = line->y1;
    u32 y2 = line->y2;
    if (y1 == y2) {
        if (y1 <= y && y < y1 + line->weight) {
            for (x = block->x + x1; x < block->x + x2; x ++) {
                if (osd_line_style_check(line->style, x)) {
                    window_line_buffer[x] = color;
                }
            }
        }
    } else if (x1 == x2) {
        for (x = 0; x < line->weight; x ++) {
            if (osd_line_style_check(line->style, y)) {
                window_line_buffer[block->x + x1 + x] = color;
            }
        }
    } else {
        double slope = (double)((int)x2 - (int)x1) / (double)((int)y2 - (int)y1);
        u32 px = x1 + (u32)(slope * (y - y1));
        if (osd_line_style_check(line->style, y)) {
            for (x = 0; x < line->weight; x++) {
                assert(block->x + px + x <= window->width);
                window_line_buffer[block->x + px + x] = color;
            }
        }
    }
}

static void osd_bitmap_paint(osd_scene *scene, osd_window *window, osd_block *block,
                             osd_ingredient *ingredient,
                             u32 *window_line_buffer,
                             u32 y) {
    osd_bitmap *bitmap = &ingredient->data.bitmap;
    if (y < bitmap->height) {
        u32 width = OSD_MIN(bitmap->width, window->width - block->x);
        u32 start = ingredient->current_bitmap * bitmap->width * bitmap->height +
                    bitmap->width * y;
        u32 x;
        for (x = start; x < start + width; x ++) {
            u32 color = osd_ingredient_get_color(scene, window,
                                                 ingredient,
                                                 ingredient->ram_data[x]);
            window_line_buffer[block->x + x - start] = color;
        }
    }
}

static u32 osd_ingredient_start_y(osd_ingredient *ingredient) {
    assert(ingredient);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE:
    case OSD_INGREDIENT_BITMAP:
        return 0;
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        return OSD_MIN(line->y1, line->y2);
    }
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
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        if (line->y2 == line->y1)
            return line->y2 - line->y1 + 1 + line->weight;
        else if (line->y2 > line->y1)
            return line->y2 - line->y1 + 1;
        else
            return line->y1 - line->y2 + 1;
    }
    case OSD_INGREDIENT_BITMAP: {
        osd_bitmap *bitmap = &ingredient->data.bitmap;
        return bitmap->height;
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
            case OSD_INGREDIENT_LINE:
                osd_line_paint(scene, window, block, ingredient,
                               window_line_buffer,
                               window_y - block->y);
                ++ paint;
                break;
            case OSD_INGREDIENT_BITMAP:
                osd_bitmap_paint(scene, window, block, ingredient,
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