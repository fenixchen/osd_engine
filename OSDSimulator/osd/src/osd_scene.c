#include "osd_scene.h"
#include "osd_common.h"
#include "osd_log.h"
#include "osd_binary.h"
#include "osd_window.h"


struct _osd_scene_priv {
    osd_scene_hw *hw;
    osd_palette *palettes[OSD_SCENE_MAX_PALETE_COUNT];
    osd_ingredient *ingredients[OSD_SCENE_MAX_INGREDIENT_COUNT];
    osd_window *windows[OSD_SCENE_MAX_WINDOW_COUNT];
    osd_binary *binary;
};


static int x_dir, y_dir;
static int first = 1;

static int screesaver_timer(osd_scene *self) {
    osd_window *window;
    int x, y, max_x, max_y;
    osd_rect rect;
    const int MOVE_STEP = 10;

    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);

    window = scene->windows[0];
    if (first) {
        first = 0;
        x_dir = osd_get_rand_boolean() ? 1 : -1;
        y_dir = osd_get_rand_boolean() ? 1 : -1;
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

static int osd_scene_timer(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);
    if (strcmp(scene->hw->title, "ScreenSaver") == 0) {
        return screesaver_timer(self);
    }
    return 0;
}

static void osd_scene_dump(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);
    OSD_LOG("scene\n\tname:%s, width:%d, height:%d, ram_offset:%#x\n",
            scene->hw->title,
            scene->hw->width, scene->hw->height, scene->hw->ram_offset);
}

static osd_ingredient* osd_scene_ingredient(osd_scene *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(index < priv->hw->ingredient_count);
    return priv->ingredients[index];
}

static osd_palette* osd_scene_palette(osd_scene *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(index < priv->hw->palette_count);
    return priv->palettes[index];
}

static const char * osd_scene_title(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->title;
}

static u16 osd_scene_timer_ms(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->timer_ms;
}

static void osd_scene_paint(osd_scene *self, fn_set_pixel set_pixel, void *arg) {
    u32 x, y, i;
    u16 width, height;
    int painted = 0;
    u32 *line_buffer, *window_line_buffer;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);

    TV_ASSERT(set_pixel);

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
                if (window->paint(window, window_line_buffer, y)) {
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

osd_rect osd_scene_rect(osd_scene *self) {
    osd_rect rect;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    rect.x = rect.y = 0;
    rect.width = priv->hw->width;
    rect.height = priv->hw->height;
    return rect;
}

static void osd_scene_destroy(osd_scene *self) {
    u32 i;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);

    for (i = 0; priv->palettes[i]; i ++) {
        osd_palette *palette = priv->palettes[i];
        FREE_OBJECT(palette);
    }

    for (i = 0; priv->ingredients[i]; i ++) {
        osd_ingredient *ingredient = priv->ingredients[i];
        FREE_OBJECT(ingredient);
    }

    for (i = 0; priv->windows[i]; i ++) {
        osd_window *window = priv->windows[i];
        window->destroy(window);
    }

    priv->binary->destroy(priv->binary);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_scene *osd_scene_create(const char *osd_file) {
    u32 count, i, ram_offset;
    osd_binary *binary;

    u8 *ram;
    osd_scene *self = MALLOC_OBJECT(osd_scene);
    osd_scene_priv *priv = MALLOC_OBJECT(osd_scene_priv);
    self->priv = priv;
    self->destroy = osd_scene_destroy;
    self->dump = osd_scene_dump;
    self->paint = osd_scene_paint;
    self->timer = osd_scene_timer;
    self->ingredient = osd_scene_ingredient;
    self->palette = osd_scene_palette;
    self->title = osd_scene_title;
    self->timer_ms = osd_scene_timer_ms;
    self->rect = osd_scene_rect;

    TV_ASSERT(osd_file);

    OSD_LOG("OSD_SCENE_DATA_SIZE:%d\n", OSD_SCENE_HW_DATA_SIZE);
    TV_ASSERT(OSD_SCENE_HW_DATA_SIZE == 8 * sizeof(u32));

    OSD_LOG("OSD_GLYPH_HEADER_SIZE:%d\n", OSD_GLYPH_HEADER_SIZE);
    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == 3 * sizeof(u32));

    OSD_LOG("OSD_INGREDIENT_DATA_SIZE:%d\n", OSD_INGREDIENT_DATA_SIZE);
    TV_ASSERT(OSD_INGREDIENT_DATA_SIZE == 4 * sizeof(u32));

    OSD_LOG("OSD_PALETTE_DATA_SIZE:%d\n", OSD_PALETTE_DATA_SIZE);
    TV_ASSERT(OSD_PALETTE_DATA_SIZE == 2 * sizeof(u32));

    OSD_LOG("OSD_WINDOW_DATA_SIZE:%d\n", OSD_WINDOW_DATA_SIZE);
    TV_ASSERT(OSD_WINDOW_DATA_SIZE == 5 * sizeof(u32));

    binary = osd_binary_create(osd_file);
    priv->binary = binary;
    ram = binary->data(binary, NULL);

    priv->hw = (osd_scene_hw *)ram;

    self->dump(self);

    ram_offset = OSD_SCENE_HW_DATA_SIZE;

    TV_ASSERT(OSD_GLYPH_HEADER_SIZE == priv->hw->glyph_header_size);
    TV_ASSERT(OSD_INGREDIENT_DATA_SIZE == priv->hw->ingredient_data_size);
    TV_ASSERT(OSD_WINDOW_DATA_SIZE == priv->hw->window_data_size);
    TV_ASSERT(OSD_PALETTE_DATA_SIZE == priv->hw->palette_data_size);

    //load palettes
    count = priv->hw->palette_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_PALETE_COUNT);
    for (i = 0; i < count; i ++) {
        osd_palette *palette = MALLOC_OBJECT(osd_palette);
        memcpy(palette, ram + ram_offset, OSD_PALETTE_DATA_SIZE);
        ram_offset += OSD_PALETTE_DATA_SIZE;
        priv->palettes[i] = palette;
        if (palette->entry_count > 0) {
            u32 *lut_ram = (u32*)(ram + palette->luts_addr);
            palette->lut = lut_ram;
        }
        log_palette(i, palette);
    }

    //load ingredients
    count = priv->hw->ingredient_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_INGREDIENT_COUNT);
    for (i = 0; i < count; i ++) {
        osd_ingredient *ingredient = MALLOC_OBJECT(osd_ingredient);
        memcpy(ingredient, ram + ram_offset, OSD_INGREDIENT_DATA_SIZE);
        ram_offset += OSD_INGREDIENT_DATA_SIZE;
        priv->ingredients[i] = ingredient;
        if (ingredient->type == OSD_INGREDIENT_BITMAP) {
            osd_bitmap *bitmap = &ingredient->data.bitmap;
            ingredient->ram_data = (u8*)(ram + bitmap->data_addr);
        } else if (ingredient->type == OSD_INGREDIENT_CHARACTER) {
            osd_character *character = &ingredient->data.character;
            ingredient->ram_data = (u8*)(ram + character->glyph_addr);
        }
        log_ingredient(i, ingredient);
    }

    //load windows
    count = priv->hw->window_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_WINDOW_COUNT);
    for (i = 0; i < count; i ++) {
        osd_window_hw *hw = (osd_window_hw *)(ram + ram_offset);
        priv->windows[i] = osd_window_create(self, hw, ram);
        ram_offset += OSD_WINDOW_DATA_SIZE;
    }

    return self;
}


