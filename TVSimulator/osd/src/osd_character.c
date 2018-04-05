#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_character.h"
#include "osd_scene.h"

struct _osd_character_priv {
    osd_character_hw *character;
    osd_scene *scene;
    osd_glyph *glyph;
    u8 *glyph_data;
};


void osd_character_paint(osd_ingredient *ingredient,
                         osd_window *window,
                         osd_block *block,
                         u32 *window_line_buffer,
                         u32 y) {
    u32 x, width, offset, col;
    u32 color;
    osd_glyph *glyph;
    u8 *glyph_data;
    osd_character_hw *character;
    osd_character *self = (osd_character *)ingredient;
    TV_TYPE_GET_PRIV(osd_character_priv, self, priv);

    character = priv->character;

    glyph = priv->glyph;
    glyph_data = priv->glyph_data;
    if (y >= glyph->height)  return;

    color = ingredient->color(ingredient,
                              character->color);
    width = TV_MIN(glyph->width, window->rect(window).width - block->x);
    offset = glyph->pitch * y;
    col = block->x + glyph->left;
    for (x = 0; x < width; x ++) {
        if (glyph->monochrome) {
            u8 pixel = glyph_data[offset + (x >> 3)];
            pixel &= (128 >> (x & 7));
            if (pixel) {
                window_line_buffer[col] = color;
            }
        } else {
            u8 alpha = glyph_data[offset + x];
            if (alpha != 0) {
                window_line_buffer[col] = osd_blend_pixel(window_line_buffer[col], color, alpha);
            }
        }
        col ++;
    }
}

static u32 osd_character_start_y(osd_ingredient *ingredient) {
    osd_character *character_self = (osd_character *)ingredient;
    TV_TYPE_GET_PRIV(osd_character_priv, character_self, priv);
    return priv->glyph->top;
}

static u32 osd_character_height(osd_ingredient *ingredient, osd_window *window) {
    osd_character *character_self = (osd_character *)ingredient;
    TV_TYPE_GET_PRIV(osd_character_priv, character_self, priv);
    return priv->glyph->height;
}

static void osd_character_dump(osd_ingredient *ingredient) {
    osd_character_hw *character;
    osd_glyph *glyph;
    osd_character *self = (osd_character *)ingredient;
    TV_TYPE_GET_PRIV(osd_character_priv, self, priv);

    character = priv->character;
    glyph = priv->glyph;
    TV_LOG("Character\tleft:%d, top:%d, width:%d, height:%d\n"
           "\tchar:%u, color:%d, size:%d, mono:%d, addr:%#x\n",
           glyph->left, glyph->top, glyph->width, glyph->height,
           (unsigned int)glyph->char_code, character->color,
           glyph->data_size, glyph->monochrome, character->glyph_addr);

}

static osd_glyph* osd_character_glyph(osd_character *self) {
    TV_TYPE_GET_PRIV(osd_character_priv, self, priv);
    return priv->glyph;
}

static void osd_character_set_glyph(osd_character *self, u32 glyph_index) {
    osd_scene *scene;
    TV_TYPE_GET_PRIV(osd_character_priv, self, priv);
    scene = priv->scene;
    priv->character->glyph_addr = scene->glyph_addr(scene, glyph_index);
    self->priv->glyph = (osd_glyph*)(scene->ram(scene) + priv->character->glyph_addr);
    self->priv->glyph_data = (u8*)self->priv->glyph + sizeof(osd_glyph);
}

static void osd_character_destroy(osd_ingredient *self) {
    osd_character *character_self = (osd_character *)self;
    FREE_OBJECT(character_self->priv);
    FREE_OBJECT(character_self);
}

osd_character *osd_character_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_character *self = MALLOC_OBJECT(osd_character);
    self->priv = MALLOC_OBJECT(osd_character_priv);
    self->priv->scene = scene;
    self->priv->character = &hw->data.character;
    self->priv->glyph = (osd_glyph*)(scene->ram(scene) + self->priv->character->glyph_addr);
    self->priv->glyph_data = (u8*)self->priv->glyph + sizeof(osd_glyph);

    self->parent.paint = osd_character_paint;
    self->parent.destroy = osd_character_destroy;
    self->parent.start_y = osd_character_start_y;
    self->parent.height = osd_character_height;
    self->parent.dump = osd_character_dump;

    self->set_glyph = osd_character_set_glyph;
    self->glyph = osd_character_glyph;
    TV_TYPE_FP_CHECK(self->set_glyph, self->set_glyph);
    return self;
}