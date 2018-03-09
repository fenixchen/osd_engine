#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "osd_types.h"
#include "osd_common.h"
#include "osd_log.h"
#include "osd_binary.h"
#include "osd_window.h"

osd_scene *osd_scene_new(const char *target_folder) {
    osd_scene *scene;
    u32 count, i, j;
    osd_binary *binary;
    assert(target_folder != NULL);

    scene = MALLOC_OBJECT(osd_scene);

    OSD_LOG("OSD_INGREDIENT_DATA_SIZE:%d", OSD_INGREDIENT_DATA_SIZE);
    assert(OSD_INGREDIENT_DATA_SIZE == 4 * sizeof(u32));

    binary = osd_binary_new(target_folder);
    if (!binary) {
        FREE_OBJECT(scene);
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
        } else if (ingredient->type == OSD_INGREDIENT_CHARACTER) {
            osd_character *character = &ingredient->data.character;
            ingredient->ram_data = (u8*)(binary->ram + character->glyph_addr - scene->ram_base_addr);
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
        FREE_OBJECT(palette->lut);
        FREE_OBJECT(palette);
    }

    for (i = 0; scene->ingredients[i]; i ++) {
        osd_ingredient *ingredient = scene->ingredients[i];
        FREE_OBJECT(ingredient);
        i ++;
    }

    for (i = 0; scene->windows[i]; i ++) {
        osd_window *window = scene->windows[i];
        FREE_OBJECT(window->blocks);
        FREE_OBJECT(window);
        i ++;
    }

    FREE_OBJECT(scene->binary);
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
        for (i = 0; scene->windows[i]; i ++) {
            osd_window *window = scene->windows[i];
            if (!window->visible) {
                continue;
            }
            if (window->y <= y && y < window->y + window->height) {
                u16 len = OSD_MIN(scene->width - window->x, window->width);
                memset(window_line_buffer, 0, sizeof(u32) * width);
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
    FREE_OBJECT(window_line_buffer);
    FREE_OBJECT(line_buffer);
}