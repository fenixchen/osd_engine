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
    TV_LOG("Label\n");
}

static void osd_label_set_int(osd_label *self, int value) {
    char buffer[16];
    t_wchar ubuffer[16];
    sprintf(buffer, "%d", value);
    tv_char_to_wchar(ubuffer, buffer);
    self->set_string(self, ubuffer);
}

static void osd_label_set_string(osd_label *self, const t_wchar *str) {
    u32 i, len, ch_index = 0;
    osd_ingredient *ingredient;
    u8 font_id, font_size;
    osd_character *character;
    osd_glyph *glyph;
    u16 glyph_blank_index;

    osd_scene *scene;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    if (priv->ingredient_count == 0) {
        return;
    }

    scene = priv->scene;
    character = (osd_character*)priv->scene->ingredient(priv->scene, priv->ingredients[0]);
    glyph = character->glyph(character);
    font_size = glyph->font_size;
    font_id = glyph->font_id;

    glyph_blank_index = scene->find_glyph(scene, L' ', font_id, font_size);
    TV_ASSERT(glyph_blank_index != OSD_SCENE_INVALID_GLYPH_INDEX);

    len = tv_wchar_len(str);
    len = TV_MIN(len, priv->ingredient_count);
    for (i = 0; i < len; i ++) {
        u16 glyph_index;
        u16 ingredient_index;
        t_wchar ch = str[i];
        ingredient_index = priv->ingredients[i];
        ingredient = priv->scene->ingredient(priv->scene, ingredient_index);

        TV_ASSERT(ingredient && ingredient->type(ingredient) == OSD_INGREDIENT_CHARACTER);

        character = (osd_character *)ingredient;

        glyph_index = priv->scene->find_glyph(priv->scene, ch, font_id, font_size);
        if (glyph_index != OSD_SCENE_INVALID_GLYPH_INDEX) {
            character->set_glyph(character, glyph_index);
        } else {
            character->set_glyph(character, glyph_blank_index);
            TV_ERR("Cannot find glyph for code(%d - %#x), font(%d), size(%d)\n", ch, ch, glyph->font_id, glyph->font_size);
        }
    }
    for (i = len; i < priv->ingredient_count; i ++) {
        u16 ingredient_index = priv->ingredients[i];
        ingredient = priv->scene->ingredient(priv->scene, ingredient_index);
        character = (osd_character *)ingredient;
        character->set_glyph(character, glyph_blank_index);
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

    self->set_int = osd_label_set_int;
    self->set_string = osd_label_set_string;

    TV_TYPE_FP_CHECK(self->set_int, self->set_int);
    return self;
}