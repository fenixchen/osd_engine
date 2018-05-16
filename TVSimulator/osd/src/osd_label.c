#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_label.h"
#include "osd_window.h"
#include "osd_text.h"
#include "osd_block.h"

struct _osd_label_priv {
    osd_window *window;
    osd_label_hw *label;
    osd_label_state **states;
    osd_block **text_blocks;
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

static void osd_label_dump(osd_ingredient *ingredient) {
    TV_LOGI("Label\n");
}

static void osd_label_set_int(osd_label *self, int value) {
    char buffer[16];
    wchar ubuffer[16];
    sprintf(buffer, "%d", value);
    tv_char_to_wchar(ubuffer, buffer);
    self->set_string(self, ubuffer);
}

static void osd_label_set_string(osd_label *self, const wchar *str) {
    osd_text *text;
    osd_block* block_text;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    block_text = priv->text_blocks[priv->label->current_text];
    text = priv->window->text(priv->window, block_text->ingredient_index(block_text));
    text->set_text(text, str);
}

static void osd_label_set_text(osd_label *self, int text_index) {
    u16 i;
    u32 color;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    TV_ASSERT(text_index < priv->label->text_count);
    priv->label->current_text = text_index;
    color = priv->states[priv->label->current_state]->color_index;
    for (i = 0; i < priv->label->text_count; i ++) {
        osd_block *block_text = priv->text_blocks[i];
        int visible = text_index == i;
        block_text->set_visible(block_text, visible);
        if (visible) {
            osd_text *text = priv->window->text(priv->window, block_text->ingredient_index(block_text));
            text->set_color(text, color);
        }
    }
}

static int osd_label_get_text(osd_label *self) {
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    return priv->label->current_text;
}

void osd_label_set_state(osd_label *self, int state) {
    u16 i;
    u32 color;
    osd_block *block_text;
    osd_text *text;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    TV_ASSERT(state < priv->label->state_count);
    if (priv->label->current_state == state) return;
    priv->label->current_state = state;
    for (i = 0; i < priv->label->state_count; i ++) {
        int visible = (i == state);
        osd_label_state *label_state = priv->states[i];
        if (label_state->bg_block_index != OSD_INVALID_BLOCK_INDEX) {
            osd_block *block = priv->window->block(priv->window, label_state->bg_block_index);
            block->set_visible(block, visible);
        }
    }

    color = priv->states[state]->color_index;

    block_text = priv->text_blocks[priv->label->current_text];
    text = priv->window->text(priv->window, block_text->ingredient_index(block_text));
    text->set_color(text, color);
}

int osd_label_get_state(osd_label *self) {
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    return priv->label->current_state;
}

static void osd_label_destroy(osd_ingredient *ingredient) {
    osd_label *self = (osd_label *)ingredient;
    TV_TYPE_GET_PRIV(osd_label_priv, self, priv);
    FREE_OBJECT(priv->states);
    FREE_OBJECT(priv->text_blocks);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

osd_label *osd_label_create(osd_window *window, osd_ingredient_hw *hw) {
    u16 i;
    osd_label_priv *priv;
    osd_label_state *label_state;
    osd_label_text *label_text;
    osd_label *self = MALLOC_OBJECT(osd_label);
    self->priv = MALLOC_OBJECT(osd_label_priv);
    priv = self->priv;
    priv->window = window;
    priv->label = &hw->data.label;

    priv->states = MALLOC_OBJECT_ARRAY(osd_label_state*, priv->label->state_count);
    label_state = (osd_label_state*)(window->ram(window) + priv->label->osd_label_data_addr);
    for (i = 0; i < priv->label->state_count; i ++) {
        self->priv->states[i] = label_state;
        ++ label_state;
    }


    priv->text_blocks = MALLOC_OBJECT_ARRAY(osd_block*, priv->label->text_count);
    label_text = (osd_label_text *)(window->ram(window) +
                                    priv->label->osd_label_data_addr +
                                    sizeof(osd_label_state) * priv->label->state_count);
    for (i = 0; i < priv->label->text_count; i ++) {
        self->priv->text_blocks[i] = window->block(window, label_text->text_block_index);
        ++ label_text;
    }

    self->parent.destroy = osd_label_destroy;
    self->parent.paint = osd_label_paint;
    self->parent.destroy = osd_label_destroy;
    self->parent.start_y = osd_label_start_y;
    self->parent.dump = osd_label_dump;

    self->set_int = osd_label_set_int;
    self->set_string = osd_label_set_string;
    self->set_text = osd_label_set_text;
    self->text = osd_label_get_text;
    self->set_state = osd_label_set_state;
    self->state = osd_label_get_state;

    TV_TYPE_FP_CHECK(self->set_int, self->state);

    self->set_state(self, priv->label->current_state);
    return self;
}