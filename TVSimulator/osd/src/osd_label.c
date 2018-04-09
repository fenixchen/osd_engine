#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_label.h"
#include "osd_scene.h"
#include "osd_character.h"
#include "osd_block.h"

#define LABEL_MAX_STATE_COUNT 16
#define LABEL_MAX_TEXT_COUNT 64

struct _osd_label_priv {
    osd_scene *scene;
    osd_label_hw *label;
    osd_label_state *state[LABEL_MAX_STATE_COUNT];
    osd_label_text *text[LABEL_MAX_TEXT_COUNT];
};


void osd_label_paint(osd_ingredient *self,
                     osd_window *window,
                     osd_block_hw *block,
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
    u8 font_id, font_size;
    osd_character *character;
    osd_glyph *glyph;
    u16 glyph_blank_index;
    u32 block_index;
    osd_label_text *label_text;
    osd_block *block;
    osd_scene *scene;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    scene = priv->scene;
    label_text = priv->text[priv->label->current_text];
    block_index = label_text->char_block_index[0];
    block = priv->scene->block(priv->scene, block_index);
    character = priv->scene->character(priv->scene, block->ingredient_index(block));

    glyph = character->glyph(character);
    font_size = glyph->font_size;
    font_id = glyph->font_id;

    glyph_blank_index = scene->find_glyph(scene, L' ', font_id, font_size);
    TV_ASSERT(glyph_blank_index != OSD_SCENE_INVALID_GLYPH_INDEX);

    len = tv_wchar_len(str);
    len = TV_MIN(len, label_text->char_block_count);
    for (i = 0; i < len; i ++) {
        u16 glyph_index;
        t_wchar ch = str[i];

        block_index = label_text->char_block_index[i];
        block = priv->scene->block(priv->scene, block_index);
        character = priv->scene->character(priv->scene, block->ingredient_index(block));

        glyph_index = priv->scene->find_glyph(priv->scene, ch, font_id, font_size);
        if (glyph_index != OSD_SCENE_INVALID_GLYPH_INDEX) {
            character->set_glyph(character, glyph_index);
        } else {
            character->set_glyph(character, glyph_blank_index);
            TV_ERR("Cannot find glyph for code(%d - %#x), font(%d), size(%d)\n", ch, ch, glyph->font_id, glyph->font_size);
        }
    }
    for (i = len; i < label_text->char_block_count; i ++) {
        block_index = label_text->char_block_index[i];
        block = priv->scene->block(priv->scene, block_index);
        character = priv->scene->character(priv->scene, block->ingredient_index(block));
        character->set_glyph(character, glyph_blank_index);
    }
}

static void osd_label_set_text(osd_label *self, int text_index) {
    u16 i, j;
    u32 color;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    TV_ASSERT(text_index < priv->label->text_count);
    priv->label->current_text = text_index;
    color = priv->state[priv->label->current_state]->color_index;
    for (i = 0; i < priv->label->text_count; i ++) {
        osd_label_text *label_text = priv->text[i];
        int visible = text_index == i;
        for (j = 0; j < label_text->char_block_count; j ++) {
            osd_block *block = priv->scene->block(priv->scene, label_text->char_block_index[j]);
            block->set_visible(block, visible);
            if (visible) {
                osd_character *character = priv->scene->character(priv->scene, block->ingredient_index(block));
                character->set_color(character, color);
            }
        }
    }
}

static int osd_label_get_text(osd_label *self) {
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    return priv->label->current_text;
}

void osd_label_set_state(osd_label *self, int state) {
    u16 i;
    u32 j, color;
    osd_label_text *label_text;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    TV_ASSERT(state < priv->label->state_count);
    if (priv->label->current_state == state) return;
    priv->label->current_state = state;
    for (i = 0; i < priv->label->state_count; i ++) {
        int visible = (i == state);
        osd_label_state *label_state = priv->state[i];
        if (label_state->bg_block_index != OSD_INVALID_BLOCK_INDEX) {
            osd_block *block = priv->scene->block(priv->scene, label_state->bg_block_index);
            block->set_visible(block, visible);
        }
    }

    color = priv->state[state]->color_index;

    label_text = priv->text[priv->label->current_text];
    for (j = 0; j < label_text->char_block_count; j ++) {
        osd_block *block = priv->scene->block(priv->scene, label_text->char_block_index[j]);
        osd_character *character = priv->scene->character(priv->scene, block->ingredient_index(block));
        character->set_color(character, color);
    }
}

int osd_label_get_state(osd_label *self) {
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    return priv->label->current_state;
}

static void osd_label_destroy(osd_ingredient *self) {
    osd_label *label_self = (osd_label *)self;
    FREE_OBJECT(label_self->priv);
    FREE_OBJECT(self);
}

osd_label *osd_label_create(osd_scene *scene, osd_ingredient_hw *hw) {
    u16 i;
    u8 *ram;
    osd_label *self = MALLOC_OBJECT(osd_label);
    self->priv = MALLOC_OBJECT(osd_label_priv);
    self->priv->scene = scene;
    self->priv->label = &hw->data.label;

    TV_ASSERT(self->priv->label->state_count < LABEL_MAX_STATE_COUNT);
    ram = scene->ram(scene) + self->priv->label->osd_label_data_addr;
    for (i = 0; i < self->priv->label->state_count; i ++) {
        self->priv->state[i] = (osd_label_state *)ram;
        ram += sizeof(osd_label_state);
    }
    for (i = 0; i < self->priv->label->text_count; i ++) {
        self->priv->text[i] = (osd_label_text*)ram;
        ram += sizeof(osd_label_text) + 4 * self->priv->text[i]->char_block_count;
    }

    self->parent.destroy = osd_label_destroy;
    self->parent.paint = osd_label_paint;
    self->parent.destroy = osd_label_destroy;
    self->parent.start_y = osd_label_start_y;
    self->parent.height = osd_label_height;
    self->parent.dump = osd_label_dump;

    self->set_int = osd_label_set_int;
    self->set_string = osd_label_set_string;
    self->set_text = osd_label_set_text;
    self->text = osd_label_get_text;
    self->set_state = osd_label_set_state;
    self->state = osd_label_get_state;

    TV_TYPE_FP_CHECK(self->set_int, self->state);
    return self;
}