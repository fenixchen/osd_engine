#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "osd_scene.h"
#include "osd_common.h"
#include "osd_log.h"
#include "osd_binary.h"
#include "osd_window.h"

osd_scene *osd_scene_new(const char *target_folder) {
    osd_scene *scene;
    u32 count, i, ram_base_addr;
    osd_binary *binary;
    u8 *ram;

    TV_ASSERT(target_folder != NULL);

    scene = MALLOC_OBJECT(osd_scene);

    OSD_LOG("OSD_GLYPH_HEADER_SIZE:%d\n", OSD_GLYPH_HEADER_SIZE);
    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == 3 * sizeof(u32));

    OSD_LOG("OSD_INGREDIENT_DATA_SIZE:%d\n", OSD_INGREDIENT_DATA_SIZE);
    TV_ASSERT(OSD_INGREDIENT_DATA_SIZE == 4 * sizeof(u32));

    OSD_LOG("OSD_PALETTE_DATA_SIZE:%d\n", OSD_PALETTE_DATA_SIZE);
    TV_ASSERT(OSD_PALETTE_DATA_SIZE == 2 * sizeof(u32));

    OSD_LOG("OSD_WINDOW_DATA_SIZE:%d\n", OSD_WINDOW_DATA_SIZE);
    TV_ASSERT(OSD_WINDOW_DATA_SIZE == 5 * sizeof(u32));

    binary = osd_binary_create(target_folder);
    ram = binary->data(binary, BINARY_TYPE_RAM);

    // load scene
    TV_ASSERT(binary->data_size(binary, BINARY_TYPE_SCENE) == OSD_SCENE_HW_DATA_SIZE);

    scene->binary = binary;
    scene->hw = (osd_scene_hw *)binary->data(binary, BINARY_TYPE_SCENE);
    ram_base_addr = scene->hw->ram_base_addr;
    log_global(scene);

    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == scene->hw->glyph_header_size);
    TV_ASSERT(OSD_INGREDIENT_DATA_SIZE == scene->hw->ingredient_data_size);
    TV_ASSERT(OSD_WINDOW_DATA_SIZE == scene->hw->window_data_size);
    TV_ASSERT(OSD_PALETTE_DATA_SIZE == scene->hw->palette_data_size);

    //load palettes
    TV_ASSERT(binary->data_size(binary, BINARY_TYPE_PALETTE) % OSD_PALETTE_DATA_SIZE == 0);
    count = binary->data_size(binary, BINARY_TYPE_PALETTE) / OSD_PALETTE_DATA_SIZE;
    TV_ASSERT(count <= OSD_SCENE_MAX_PALETE_COUNT);
    for (i = 0; i < count; i ++) {
        osd_palette *palette = MALLOC_OBJECT(osd_palette);
        memcpy(palette, binary->data(binary, BINARY_TYPE_PALETTE) + i * OSD_PALETTE_DATA_SIZE, OSD_PALETTE_DATA_SIZE);
        scene->palettes[i] = palette;
        if (palette->entry_count > 0) {
            u32 *lut_ram = (u32*)(ram + palette->luts_addr - ram_base_addr);
            palette->lut = lut_ram;
        }
        log_palette(i, palette);
    }

    //load ingredients
    TV_ASSERT(binary->data_size(binary, BINARY_TYPE_INGREDIENT) % OSD_INGREDIENT_DATA_SIZE == 0);
    TV_ASSERT(count <= OSD_SCENE_MAX_INGREDIENT_COUNT);
    count = binary->data_size(binary, BINARY_TYPE_INGREDIENT) / OSD_INGREDIENT_DATA_SIZE;
    for (i = 0; i < count; i ++) {
        osd_ingredient *ingredient = MALLOC_OBJECT(osd_ingredient);
        memcpy(ingredient,
               binary->data(binary,BINARY_TYPE_INGREDIENT) + i * OSD_INGREDIENT_DATA_SIZE,
               OSD_INGREDIENT_DATA_SIZE);
        scene->ingredients[i] = ingredient;
        if (ingredient->type == OSD_INGREDIENT_BITMAP) {
            osd_bitmap *bitmap = &ingredient->data.bitmap;
            ingredient->ram_data = (u8*)(ram + bitmap->data_addr - ram_base_addr);
        } else if (ingredient->type == OSD_INGREDIENT_CHARACTER) {
            osd_character *character = &ingredient->data.character;
            ingredient->ram_data = (u8*)(ram + character->glyph_addr - ram_base_addr);
        }
        log_ingredient(i, ingredient);
    }

    //load windows
    TV_ASSERT(binary->data_size(binary, BINARY_TYPE_WINDOW) % OSD_WINDOW_DATA_SIZE == 0);
    count = binary->data_size(binary, BINARY_TYPE_WINDOW) / OSD_WINDOW_DATA_SIZE;
    TV_ASSERT(count <= OSD_SCENE_MAX_WINDOW_COUNT);
    for (i = 0; i < count; i ++) {
        osd_window_hw *hw = (osd_window_hw*)(binary->data(binary, BINARY_TYPE_WINDOW)
                                             + i * OSD_WINDOW_DATA_SIZE);
        scene->windows[i] = osd_window_create(hw, ram, ram_base_addr);
    }

    return scene;
}

void osd_scene_delete(osd_scene *scene) {
    u32 i;
    TV_ASSERT(scene);

    for (i = 0; scene->palettes[i]; i ++) {
        osd_palette *palette = scene->palettes[i];
        FREE_OBJECT(palette);
    }

    for (i = 0; scene->ingredients[i]; i ++) {
        osd_ingredient *ingredient = scene->ingredients[i];
        FREE_OBJECT(ingredient);
    }

    for (i = 0; scene->windows[i]; i ++) {
        osd_window *window = scene->windows[i];
        window->destroy(window);
    }

    scene->binary->destroy(scene->binary);
    FREE_OBJECT(scene);
}

void osd_scene_paint(osd_scene *scene, u32 frame, fn_set_pixel set_pixel, void *arg) {
    u32 x, y, i;
    u16 width, height;
    int painted = 0;
    u32 *line_buffer, *window_line_buffer;

    TV_ASSERT(set_pixel && scene);

    width = scene->hw->width;
    height = scene->hw->height;
    line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    window_line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    for (y = 0; y < height; y ++) {
        memset(line_buffer, 0, sizeof(u32) * width);
        for (i = 0; scene->windows[i]; i ++) {
            osd_window *window = scene->windows[i];
            osd_rect rect;
            if (!window->is_visible(window)) {
                continue;
            }
            rect = window->get_rect(window);

            if (rect.y <= y && y < rect.y + rect.height) {
                u16 len = OSD_MIN(scene->hw->width - rect.x, rect.width);
                memset(window_line_buffer, 0, sizeof(u32) * width);
                if (window->paint(window, scene, window_line_buffer, y)) {
                    osd_merge_line(line_buffer, window_line_buffer,
                                   len, rect.x, window->get_alpha(window));
                    painted = 1;
                }
            }
        }
        if (painted) {
            for (x = 0; x < width; x ++) {
                if (line_buffer[x] != 0) {
                    set_pixel(arg, x, y, line_buffer[x]);
                }
            }
        }
    }
    FREE_OBJECT(window_line_buffer);
    FREE_OBJECT(line_buffer);
}

static int get_rand_boolean(void) {
    int r;
    static int first = 1;
    if (first) {
        srand((unsigned)time(NULL));
        first = 0;
    }
    r = rand();
    return rand() > (RAND_MAX / 2);
}

static int screesaver_timer(osd_scene *scene) {
    osd_window *window = scene->windows[0];
    int x, y, max_x, max_y;
    osd_rect rect;
    const int MOVE_STEP = 10;
    static int x_dir, y_dir;
    static int first = 1;
    if (first) {
        first = 0;
        x_dir = get_rand_boolean() ? 1 : -1;
        y_dir = get_rand_boolean() ? 1 : -1;
    }

    rect = window->get_rect(window);
    max_x = scene->hw->width - rect.width - 1;
    max_y = scene->hw->height - rect.height - 1;

    x = OSD_MIN(OSD_MAX(0, (int)rect.x + MOVE_STEP * x_dir), max_x);
    y = OSD_MIN(OSD_MAX(0, (int)rect.y + MOVE_STEP * y_dir), max_y);
    window->move_to(window, x, y);

    if (x == 0 || x == max_x) {
        x_dir = -x_dir;
    }

    if (y == 0 || y == max_y) {
        y_dir = -y_dir;
    }
    return 1;
}

int osd_scene_timer(osd_scene *scene) {
    if (strcmp(scene->hw->title, "ScreenSaver") == 0) {
        return screesaver_timer(scene);
    }
    return 0;
}