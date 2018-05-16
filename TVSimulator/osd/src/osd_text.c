#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_text.h"
#include "osd_scene.h"
#include "osd_glyph.h"

struct _osd_text_priv {
    osd_window *window;
    osd_ingredient_hw *ingredient_hw;
    osd_text_hw *hw;
    osd_text_data *text_data;
};


void osd_text_paint(osd_ingredient *ingredient,
                    osd_window *window,
                    osd_block_hw *block,
                    u32 *window_line_buffer,
                    u32 y) {
    s32 left, col, row, offset;
    u32 i, x;
    u32 color;
    s32 window_width = window->rect(window).width;
    osd_text *self = (osd_text *)ingredient;
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    color = ingredient->color(ingredient, priv->hw->color);
    left = block->x;
    for (i = 0; i < priv->hw->length; i ++) {
        u16 glyph_index = priv->text_data[i].glyph_index;
        osd_glyph *glyph = window->glyph(window, glyph_index);
        osd_glyph_hw *glyph_hw = glyph->hw(glyph);
        u8 *glyph_data = window->ram(window) + glyph_hw->glyph_data;
        col = left + glyph_hw->left;
        left += glyph_hw->advance_x;
        row = y - glyph_hw->top + priv->hw->top_line;
        if (row < 0 || row >= glyph_hw->height) {
            continue;
        }
        offset = row * glyph_hw->pitch;
        for (x = 0; x < glyph_hw->width; x ++) {
            if (glyph_hw->monochrome) {
                u8 bit = glyph_data[offset + (x >> 3)];
                bit = bit & (128 >> (x & 7));
                if (bit && TV_BETWEEN(col, 0, window_width)) {
                    window_line_buffer[col] = color;
                }
            } else {
                u8 intensity = glyph_data[offset + x];
                if (intensity > 0 && TV_BETWEEN(col, 0, window_width)) {
                    window_line_buffer[col] = osd_blend_pixel(window_line_buffer[col], color, intensity);
                }
            }
            col ++;
        }
    }
}

void osd_text_set_text(osd_text *self, const wchar *str) {
    int len, i;
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    len = tv_wchar_len(str);
    len = TV_MIN(len, priv->hw->length);

    for (i = 0; i < len; i ++) {
        wchar ch = str[i];
        u16 glyph_index = priv->window->find_glyph(priv->window, ch, priv->hw->font_id, priv->hw->font_size);
        if (glyph_index == OSD_INVALID_GLYPH_INDEX) {
            TV_LOGE("Cannot find char_code(%d), font_id(%d), font_size(%d) from glyph of window\n",
                    ch, priv->hw->font_id, priv->hw->font_size);
            TV_ASSERT(glyph_index != OSD_INVALID_GLYPH_INDEX);
        }
        priv->text_data[i].glyph_index = glyph_index;
    }
}

static u16 osd_text_length(osd_text *self) {
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    return priv->hw->length;
}

static u8 osd_text_font_id(osd_text *self) {
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    return priv->hw->font_id;
}
static u8 osd_text_font_size(osd_text *self) {
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    return priv->hw->font_size;
}

static u32 osd_text_start_y(osd_ingredient *ingredient) {
    osd_text *self = (osd_text *)ingredient;
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    return priv->hw->top_line;
}

static void osd_text_dump(osd_ingredient *ingredient) {
    osd_text *self = (osd_text *)ingredient;
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);

    TV_LOGI("Text\tcolor:%d, length:%d, top_line:%d\n",
            priv->hw->color, priv->hw->length, priv->hw->top_line);
}

static void osd_text_set_color(osd_text *self, u32 color) {
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    priv->hw->color = color;
}
static u32 osd_text_color(osd_text *self) {
    TV_TYPE_GET_PRIV(osd_text_priv, self, priv);
    return priv->hw->color;
}


static void osd_text_destroy(osd_ingredient *self) {
    osd_text *character_self = (osd_text *)self;
    FREE_OBJECT(character_self->priv);
    FREE_OBJECT(character_self);
}

osd_text *osd_text_create(osd_window *window, osd_ingredient_hw *hw) {
    osd_text_priv *priv;
    osd_text *self = MALLOC_OBJECT(osd_text);
    self->priv = MALLOC_OBJECT(osd_text_priv);
    priv = self->priv;
    priv->window = window;
    priv->ingredient_hw = hw;
    priv->hw = &priv->ingredient_hw->data.text;
    priv->text_data = (osd_text_data*)(window->ram(window) + priv->hw->text_data_addr);

    self->parent.paint = osd_text_paint;
    self->parent.destroy = osd_text_destroy;
    self->parent.start_y = osd_text_start_y;
    self->parent.dump = osd_text_dump;

    self->font_id = osd_text_font_id;
    self->font_size = osd_text_font_size;
    self->length = osd_text_length;
    self->set_text = osd_text_set_text;
    self->set_color = osd_text_set_color;
    self->color = osd_text_color;
    TV_TYPE_FP_CHECK(self->font_id, self->color);

    return self;
}