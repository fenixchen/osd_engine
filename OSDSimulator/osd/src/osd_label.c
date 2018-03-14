#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_label.h"
#include "osd_scene.h"
#include "osd_character.h"

struct _osd_label_priv {
    osd_scene *scene;
    u32 ingredient_count;
    u16 *ingredients;
};


void osd_label_paint(osd_ingredient *self,
                     osd_window *window,
                     osd_block *block,
                     u32 *window_label_buffer,
                     u32 y) {
}


static u32 osd_label_start_y(osd_ingredient *self) {
    return 0;
}

static u32 osd_label_height(osd_ingredient *self, osd_window *window) {
    return 0;
}

static void osd_label_dump(osd_ingredient *ingredient) {
    OSD_LOG("Label\n");
}

static void osd_label_set_value(osd_label *self, int value) {
    u32 i, len;
    char buffer[16], format[16];
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);

    if (priv->ingredient_count == 0) {
        return;
    }

    sprintf(format, "%%%dd", priv->ingredient_count);
    len = sprintf(buffer, format, value);
    len = OSD_MIN(len, priv->ingredient_count);

    for (i = 0; i < len; i ++) {
        osd_character *character;
        osd_glyph *glyph;
        u16 glyph_index;
        char ch = buffer[i];
        u16 ingredient_index = priv->ingredients[i];
        osd_ingredient *ingredient = priv->scene->ingredient(priv->scene, ingredient_index);

        TV_ASSERT(ingredient && ingredient->type(ingredient) == OSD_INGREDIENT_CHARACTER);

        character = (osd_character *)ingredient;
        glyph = character->glyph(character);

        glyph_index = priv->scene->find_glyph(priv->scene, ch, glyph->font_id, glyph->font_size);
        if (glyph_index != OSD_SCENE_INVALID_GLYPH_INDEX) {
            character->set_glyph(character, glyph_index);
        }
    }
}

static void osd_label_destroy(osd_ingredient *self) {
    osd_label *label_self = (osd_label *)self;
    FREE_OBJECT(label_self->priv);
    FREE_OBJECT(self);
}

osd_label *osd_label_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_label *self = MALLOC_OBJECT(osd_label);
    self->priv = MALLOC_OBJECT(osd_label_priv);
    self->priv->scene = scene;
    self->priv->ingredient_count = hw->data.label.ingredient_count;
    self->priv->ingredients =
        (u16 *)(scene->ram(scene) + hw->data.label.ingredient_addr);

    self->parent.destroy = osd_label_destroy;
    self->parent.paint = osd_label_paint;
    self->parent.destroy = osd_label_destroy;
    self->parent.start_y = osd_label_start_y;
    self->parent.height = osd_label_height;
    self->parent.dump = osd_label_dump;

    self->set_value = osd_label_set_value;

    TV_TYPE_FP_CHECK(self->set_value, self->set_value);
    return self;
}