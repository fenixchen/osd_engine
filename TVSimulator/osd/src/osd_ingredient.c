#include "osd_ingredient.h"
#include "osd_rectangle.h"
#include "osd_line.h"
#include "osd_bitmap.h"
#include "osd_character.h"
#include "osd_window.h"
#include "osd_scene.h"
#include "osd_palette.h"
#include "osd_bitmap.h"
#include "osd_rectangle.h"
#include "osd_line.h"
#include "osd_character.h"
#include "osd_label.h"

struct _osd_ingredient_priv {
    osd_ingredient_hw *hw;
    osd_scene *scene;
    void (*child_destroy)(osd_ingredient *self);
};

u32 osd_ingredient_palette_index(osd_ingredient *self) {
    TV_TYPE_GET_PRIV(osd_ingredient_priv, self, priv);
    return priv->hw->palette_index;
}

u32 osd_ingredient_color(osd_ingredient *self, u32 index) {
    osd_palette *palette;
    u8 palette_index;
    TV_TYPE_GET_PRIV(osd_ingredient_priv, self, priv);
    palette_index = priv->hw->palette_index;
    TV_ASSERT(palette_index != OSD_PALETTE_INDEX_INVALID);
    palette = priv->scene->palette(priv->scene, palette_index);
    return palette->color(palette, index);
}


u32 osd_ingredient_color2(osd_ingredient *self, u8 *color_ram, u32 index) {

    u32 color_index;
    u8 pixel_bits;
    osd_palette *palette;
    u8 palette_index;
    TV_TYPE_GET_PRIV(osd_ingredient_priv, self, priv);

    palette_index = priv->hw->palette_index;
    TV_ASSERT(palette_index != OSD_PALETTE_INDEX_INVALID);
    palette = priv->scene->palette(priv->scene, palette_index);
    pixel_bits = palette->pixel_bits(palette);
    if (pixel_bits == 8) {
        color_index = color_ram[index];
    } else if (pixel_bits == 16) {
        color_index = (color_ram[index * 2 + 1] << 8) | color_ram[index * 2];
    } else {
        TV_ASSERT(0);
    }
    return palette->color(palette, color_index);
}

u8 osd_ingredient_type(osd_ingredient *self) {
    TV_TYPE_GET_PRIV(osd_ingredient_priv, self, priv);
    return priv->hw->type;
}

static void osd_ingredient_destroy(osd_ingredient *self) {
    void (*child_destroy)(osd_ingredient *self);
    TV_TYPE_GET_PRIV(osd_ingredient_priv, self, priv);
    child_destroy = priv->child_destroy;
    FREE_OBJECT(priv);
    child_destroy(self);
}

osd_ingredient *osd_ingredient_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_ingredient *self;
    osd_ingredient_priv *priv;

    switch (hw->type) {
    case OSD_INGREDIENT_BITMAP:
        self = &osd_bitmap_create(scene, hw)->parent;
        break;
    case OSD_INGREDIENT_LINE:
        self = &osd_line_create(scene, hw)->parent;
        break;
    case OSD_INGREDIENT_RECTANGLE:
        self = &osd_rectangle_create(scene, hw)->parent;
        break;
    case OSD_INGREDIENT_CHARACTER:
        self = &osd_character_create(scene, hw)->parent;
        break;
    case OSD_INGREDIENT_LABEL:
    case OSD_INGREDIENT_FORM:
    case OSD_INGREDIENT_BUTTON:
    case OSD_INGREDIENT_EDIT:
        self = &osd_label_create(scene, hw)->parent;
        break;
    default:
        TV_ASSERT(0);
    }
    priv = MALLOC_OBJECT(osd_ingredient_priv);

    self->priv = priv;
    priv->hw = hw;
    priv->scene = scene;
    priv->child_destroy = self->destroy;
    self->destroy = osd_ingredient_destroy;
    self->color = osd_ingredient_color;
    self->color2 = osd_ingredient_color2;
    self->palette_index = osd_ingredient_palette_index;
    self->type = osd_ingredient_type;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);
    return self;
}
