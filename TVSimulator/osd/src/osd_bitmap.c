#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"
#include "osd_window.h"
#include "osd_bitmap.h"
#include "osd_scene.h"

struct _osd_bitmap_priv {
    osd_bitmap_hw *bitmap;
    u8 bitmap_count;
    osd_bitmap_data *data;
};

static void osd_bitmap_paint(osd_ingredient *self,
                             osd_window *window,
                             osd_block *block,
                             u32 *window_line_buffer,
                             u32 y) {
    u32 width, start, x, cx;
    u32 bitmap_width, bitmap_height;
    u8 *bitmap_data;
    osd_bitmap_hw *bitmap;
    osd_bitmap *bitmap_self = (osd_bitmap *)self;
    TV_TYPE_GET_PRIV(osd_bitmap_priv, bitmap_self, priv);

    bitmap = priv->bitmap;
    if (y >= bitmap->height) return;
    bitmap_data = (u8*)priv->data + sizeof(osd_bitmap_data);
    bitmap_width = priv->data->bitmap_width;
    bitmap_height = priv->data->bitmap_height;

    width = TV_MIN(bitmap->width, window->rect(window).width - block->x);
    if (!bitmap->tiled) {
        width = TV_MIN(width, bitmap_width);
    }

    start = bitmap->current_bitmap * bitmap_width * bitmap_height +
            bitmap_width * (y % bitmap_height);

    cx = start;
    for (x = start; x < start + width; x ++) {
        u32 col = block->x + x - start;
        u32 color = self->color2(self,
                                 window,
                                 bitmap_data,
                                 cx);
        if (bitmap->mask_color == 0) {
            if (!bitmap->transparent || priv->data->transparent_color != color)
                window_line_buffer[col] = color;
        } else {
            u8 alpha = color & 0xFF;
            if (alpha > 0) {
                color = self->color(self, window, bitmap->mask_color);
                window_line_buffer[col] = osd_blend_pixel(window_line_buffer[col], color, alpha);
            }
        }
        cx ++;
        if (cx - start >= bitmap_width) {
            cx = start;
        }
    }
}

static u32 osd_bitmap_start_y(osd_ingredient *self) {
    return 0;
}

static u32 osd_bitmap_height(osd_ingredient *self, osd_window *window) {
    osd_bitmap *bitmap_self = (osd_bitmap *)self;
    return bitmap_self->priv->bitmap->height;
}

static void osd_bitmap_dump(osd_ingredient *ingredient) {
    osd_bitmap_hw *bitmap;
    osd_bitmap *self = (osd_bitmap *)ingredient;
    TV_TYPE_GET_PRIV(osd_bitmap_priv, self, priv);

    bitmap = priv->bitmap;
    TV_LOG("Bitmap\n\tpalette:%d, width:%d, height:%d, count:%d, addr:%#x, size:%d\n\t"
           "bitmap_width:%d, bitmap_height:%d, tiled:%d, mask_color:%d\n",
           ingredient->palette_index(ingredient),
           bitmap->width, bitmap->height, bitmap->bitmap_count,
           bitmap->data_addr, priv->data->data_size,
           priv->data->bitmap_width, priv->data->bitmap_height,
           bitmap->tiled, bitmap->mask_color);
}

static void osd_bitmap_set_current(osd_bitmap *self, u8 bitmap_index) {
    TV_TYPE_GET_PRIV(osd_bitmap_priv, self, priv);
    if (bitmap_index < priv->bitmap->bitmap_count) {
        priv->bitmap->current_bitmap = bitmap_index;
    }
}

static u8 osd_bitmap_current(osd_bitmap *self) {
    TV_TYPE_GET_PRIV(osd_bitmap_priv, self, priv);
    return priv->bitmap->current_bitmap;
}

static u8 osd_bitmap_count(osd_bitmap *self) {
    TV_TYPE_GET_PRIV(osd_bitmap_priv, self, priv);
    return priv->bitmap->bitmap_count;
}

static void osd_bitmap_destroy(osd_ingredient *self) {
    osd_bitmap *bitmap_self = (osd_bitmap *)self;
    FREE_OBJECT(bitmap_self->priv);
    FREE_OBJECT(self);
}

osd_bitmap *osd_bitmap_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_bitmap *self = MALLOC_OBJECT(osd_bitmap);

    self->priv = MALLOC_OBJECT(osd_bitmap_priv);

    self->priv->bitmap = &hw->data.bitmap;
    self->priv->data = (osd_bitmap_data*)(scene->ram(scene) + self->priv->bitmap->data_addr);

    self->parent.destroy = osd_bitmap_destroy;
    self->parent.paint = osd_bitmap_paint;
    self->parent.destroy = osd_bitmap_destroy;
    self->parent.start_y = osd_bitmap_start_y;
    self->parent.height = osd_bitmap_height;
    self->parent.dump = osd_bitmap_dump;

    self->set_current = osd_bitmap_set_current;
    self->current = osd_bitmap_current;
    self->count = osd_bitmap_count;
    TV_TYPE_FP_CHECK(self->set_current, self->count);
    return self;
}