#include "osd_scene.h"
#include "osd_common.h"
#include "osd_binary.h"
#include "osd_window.h"
#include "osd_palette.h"
#include "osd_ingredient.h"
#include "osd_label.h"
#include "osd_app.h"

struct _osd_scene_priv {
    osd_scene_hw *hw;
    osd_app *app;
    osd_palette *palettes[OSD_SCENE_MAX_PALETE_COUNT];
    osd_ingredient *ingredients[OSD_SCENE_MAX_INGREDIENT_COUNT];
    osd_window *windows[OSD_SCENE_MAX_WINDOW_COUNT];
    osd_glyph *glyphs[OSD_SCENE_MAX_GLYPH_COUNT];
    osd_binary *binary;
};


static int osd_scene_trigger(osd_scene *self, osd_trigger_type type,
                             osd_trigger_data *data) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    if (priv->app) {
        return priv->app->event(priv->app, type, data);
    }
    return 0;
}

static int osd_scene_timer_interval(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->timer_ms;
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

static osd_window* osd_scene_window(osd_scene *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(index < priv->hw->window_count);
    return priv->windows[index];
}

static const char * osd_scene_title(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->title;
}

static u16 osd_scene_timer_ms(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->hw->timer_ms;
}

static void osd_scene_paint(osd_scene *self,
                            fn_set_pixel set_pixel, void *arg,
                            u32 *framebuffer) {
    u32 x, y, i;
    u16 width, height;
    int painted = 0;
    u32 *line_buffer, *window_line_buffer;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, scene);

    width = scene->hw->width;
    height = scene->hw->height;

    line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    window_line_buffer = MALLOC_OBJECT_ARRAY(u32, width);
    for (y = 0; y < height; y ++) {
        memset(line_buffer, 0, sizeof(u32) * width);
        for (i = 0; scene->windows[i]; i ++) {
            osd_window *window = scene->windows[i];
            osd_rect rect;
            if (!window->visible(window)) {
                continue;
            }
            rect = window->rect(window);

            if (rect.y <= y && y < rect.y + rect.height) {
                u16 len = OSD_MIN(scene->hw->width - rect.x, rect.width);
                memset(window_line_buffer, 0, sizeof(u32) * width);
                if (window->paint(window, window_line_buffer, y)) {
                    osd_merge_line(line_buffer, window_line_buffer,
                                   len, rect.x, window->alpha(window));
                    painted = 1;
                }
            }
        }
        if (painted && set_pixel) {
            for (x = 0; x < width; x ++) {
                if (line_buffer[x] != 0) {
                    set_pixel(arg, x, y, line_buffer[x]);
                }
            }
        }
        if (framebuffer) {
            memcpy(framebuffer, line_buffer, sizeof(u32) * width);
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

static u8* osd_scene_ram(osd_scene *self) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    return priv->binary->data(priv->binary, NULL);
}

static u32 osd_scene_glyph_addr(osd_scene *self, u16 index) {
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    TV_ASSERT(index < priv->hw->glyph_count);
    return (u8*)priv->glyphs[index] - osd_scene_ram(self);
}

static u16 osd_scene_find_glyph(osd_scene *self, u16 char_code,
                                u8 font_id, u8 font_size) {
    u16 i;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);
    for (i = 0; i < priv->hw->glyph_count; i ++) {
        osd_glyph *glyph = priv->glyphs[i];
        TV_ASSERT(glyph);
        if (glyph->font_id == font_id
                && glyph->char_code == char_code
                && glyph->font_size == font_size) {
            return i;
        }
    }
    return OSD_SCENE_INVALID_GLYPH_INDEX;
}

static void osd_scene_destroy(osd_scene *self) {
    u32 i;
    TV_TYPE_GET_PRIV(osd_scene_priv, self, priv);

    for (i = 0; priv->palettes[i]; i ++) {
        osd_palette *palette = priv->palettes[i];
        palette->destroy(palette);
    }

    for (i = 0; priv->ingredients[i]; i ++) {
        osd_ingredient *ingredient = priv->ingredients[i];
        ingredient->destroy(ingredient);
    }

    for (i = 0; priv->windows[i]; i ++) {
        osd_window *window = priv->windows[i];
        window->destroy(window);
    }

    priv->binary->destroy(priv->binary);

    if (priv->app) {
        priv->app->destroy(priv->app);
    }
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_scene *osd_scene_create(const char *osd_file, osd_app *app) {
    u32 count, i, ram_offset;
    osd_binary *binary;

    u8 *ram;
    osd_scene *self = MALLOC_OBJECT(osd_scene);
    osd_scene_priv *priv = MALLOC_OBJECT(osd_scene_priv);
    self->priv = priv;
    priv->app = app;
    self->destroy = osd_scene_destroy;
    self->paint = osd_scene_paint;
    self->timer_interval = osd_scene_timer_interval;
    self->ingredient = osd_scene_ingredient;
    self->palette = osd_scene_palette;
    self->window = osd_scene_window;
    self->title = osd_scene_title;
    self->trigger = osd_scene_trigger;
    self->rect = osd_scene_rect;
    self->ram = osd_scene_ram;
    self->glyph_addr = osd_scene_glyph_addr;
    self->find_glyph = osd_scene_find_glyph;
    self->dump = osd_scene_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);

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
        osd_palette_hw *hw = (osd_palette_hw *)(ram + ram_offset);
        osd_palette *palette = osd_palette_create(self, hw);
        ram_offset += OSD_PALETTE_DATA_SIZE;
        priv->palettes[i] = palette;
        palette->dump(palette);
    }

    //load ingredients
    count = priv->hw->ingredient_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_INGREDIENT_COUNT);
    for (i = 0; i < count; i ++) {
        osd_ingredient_hw *hw = (osd_ingredient_hw*)(ram + ram_offset);
        osd_ingredient *ingredient = osd_ingredient_create(self, hw);
        ram_offset += OSD_INGREDIENT_DATA_SIZE;
        if (ingredient) {
            priv->ingredients[i] = ingredient;
            ingredient->dump(ingredient);
        }
    }

    //load windows
    count = priv->hw->window_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_WINDOW_COUNT);
    for (i = 0; i < count; i ++) {
        osd_window_hw *hw = (osd_window_hw *)(ram + ram_offset);
        priv->windows[i] = osd_window_create(self, hw);
        ram_offset += OSD_WINDOW_DATA_SIZE;
        priv->windows[i]->dump(priv->windows[i]);
    }

    count = priv->hw->glyph_count;
    TV_ASSERT(count <= OSD_SCENE_MAX_GLYPH_COUNT);
    for (i = 0; i < count; i ++) {
        osd_glyph *hw = (osd_glyph *)(ram + ram_offset);
        priv->glyphs[i] = hw;
        ram_offset += OSD_GLYPH_HEADER_SIZE + hw->data_size;
    }

    {
        if (strcmp(priv->hw->title, "ScreenSaver") == 0) {
            extern osd_app *osd_app_screensaver_create(osd_scene *scene);
            priv->app = osd_app_screensaver_create(self);
        }
    }

    return self;
}


