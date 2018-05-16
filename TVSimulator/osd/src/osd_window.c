#include "tv_api.h"
#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_block.h"
#include "osd_palette.h"
#include "osd_label.h"
#include "osd_glyph.h"

struct _osd_window_priv {
    osd_window_hw *hw;
    u32 ingredient_count;
    osd_scene *scene;
    osd_block** blocks;
    osd_palette** palettes;
    osd_ingredient** ingredients;
    osd_glyph** glyphs;
    osd_window_proc *window_proc;
};

static int osd_window_paint(osd_window *self,
                            u32 *window_line_buffer,
                            u16 y) {
    u32 i;
    u16 window_y;
    int paint = 0;
    osd_window_hw *window;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    window = priv->hw;
    window_y = y - window->y;
    for (i = 0; i < window->block_count; i ++) {
        osd_ingredient *ingredient;
        s32 block_start_y, block_height;
        osd_block_hw *block = priv->blocks[i]->hw(priv->blocks[i]);
        if (!block->visible) {
            continue;
        }
        ingredient = self->ingredient(self, block->ingredient_index);
        block_start_y = block->y + ingredient->start_y(ingredient);
        block_height = block->height;
        if (block_start_y <= window_y && window_y < block_start_y + block_height) {
            ingredient->paint(ingredient, self, block,
                              window_line_buffer,
                              window_y - block_start_y);
            ++ paint;
        }
    }
    return paint;
}

static void osd_window_dump(osd_window *self) {
    osd_window_hw *window;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    window = priv->hw;

    TV_LOGI("window\n"
            "\tvisible:%d, alpha:%d, z_order:%d\n"
            "\tx:%d, y:%d, width:%d, height:%d\n",
            window->visible, window->alpha, window->z_order,
            window->x, window->y, window->width, window->height);
    TV_LOGI("\tblock_count:%d, block_addr:%#x\n",
            window->block_count, (unsigned int)priv->blocks);

}

static osd_glyph* osd_window_glyph(osd_window *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->hw->glyph_count);
    return priv->glyphs[index];
}

static osd_block* osd_window_block(osd_window *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->hw->block_count);
    return priv->blocks[index];
}

static osd_rect osd_window_rect(osd_window *self) {
    osd_rect rect;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    rect.x = priv->hw->x;
    rect.y = priv->hw->y;
    rect.width = priv->hw->width;
    rect.height = priv->hw->height;
    return rect;
}

static void	osd_window_set_rect(osd_window *self, osd_rect *rect) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->hw->x = rect->x;
    priv->hw->y = rect->y;
    priv->hw->width = rect->width;
    priv->hw->height = rect->height;
}

static u8 osd_window_alpha(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->hw->alpha;
}

static int osd_window_visible(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->hw->visible;
}

static void osd_window_set_visible(osd_window *self, int visible) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->hw->visible = visible;
}

void osd_window_set_window_proc(osd_window *self, osd_window_proc *window_proc) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->window_proc = window_proc;
}

int	osd_window_send_message(osd_window *self, osd_event_type type, osd_event_data *data) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    if (priv->window_proc && priv->window_proc->handler) {
        osd_window_proc *window_proc = priv->window_proc;
        return window_proc->handler(window_proc, type, data);
    } else {
        return 0;
    }
}

static void osd_window_destroy(osd_window *self) {
    u32 i;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);

    for (i = 0; i < priv->hw->block_count; i ++) {
        TV_TYPE_DESTROY(priv->blocks[i]);
    }
    FREE_OBJECT(priv->blocks);


    for (i = 0; i < priv->hw->palette_count; i ++) {
        TV_TYPE_DESTROY(priv->palettes[i]);
    }
    FREE_OBJECT(priv->palettes);

    for (i = 0; i < priv->ingredient_count; i ++) {
        TV_TYPE_DESTROY(priv->ingredients[i]);
    }
    FREE_OBJECT(priv->ingredients);

    for (i = 0; i < priv->hw->glyph_count; i ++) {
        TV_TYPE_DESTROY(priv->glyphs[i]);
    }
    FREE_OBJECT(priv->glyphs);

    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

static void osd_window_move_to(osd_window *self, int x, int y) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->hw->x = x;
    priv->hw->y = y;
}


static u8* osd_window_ram(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->scene->ram(priv->scene);
}

static osd_ingredient* osd_window_ingredient(osd_window *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    return priv->ingredients[index];
}

static osd_text* osd_window_text(osd_window *self, u32 index) {
    osd_ingredient *ingredient;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    ingredient = priv->ingredients[index];
    TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_TEXT);
    return (osd_text*)ingredient;
}

static osd_bitmap* osd_window_bitmap(osd_window *self, u32 index) {
    osd_ingredient *ingredient;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    ingredient = priv->ingredients[index];
    TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_BITMAP);
    return (osd_bitmap*)ingredient;
}

static osd_rectangle* osd_window_rectangle(osd_window *self, u32 index) {
    osd_ingredient *ingredient;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    ingredient = priv->ingredients[index];
    TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_RECTANGLE);
    return (osd_rectangle*)ingredient;
}

static osd_line* osd_window_line(osd_window *self, u32 index) {
    osd_ingredient *ingredient;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    ingredient = priv->ingredients[index];
    TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_LINE);
    return (osd_line*)ingredient;
}

static osd_label* osd_window_label(osd_window *self, u32 index) {
    osd_ingredient *ingredient;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->ingredient_count);
    ingredient = priv->ingredients[index];
    TV_ASSERT(ingredient->type(ingredient) == OSD_INGREDIENT_LABEL);
    return (osd_label*)ingredient;
}

static osd_palette* osd_window_palette(osd_window *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    TV_ASSERT(index < priv->hw->palette_count);
    return priv->palettes[index];
}

static u16 osd_window_find_glyph(osd_window *self, u16 char_code,
                                 u8 font_id, u8 font_size) {
    u16 i;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    for (i = 0; i < priv->hw->glyph_count; i ++) {
        osd_glyph_hw *glyph = priv->glyphs[i]->hw(priv->glyphs[i]);
        TV_ASSERT(glyph);
        if (glyph->font_id == font_id
                && glyph->char_code == char_code
                && glyph->font_size == font_size) {
            return i;
        }
    }
    return OSD_INVALID_GLYPH_INDEX;
}

osd_window *osd_window_create(osd_scene *scene, osd_window_hw *hw) {
    u32 i;
    osd_window_priv *priv;

    osd_window *self = MALLOC_OBJECT(osd_window);
    priv = MALLOC_OBJECT(osd_window_priv);
    priv->scene = scene;
    self->priv = priv;
    self->destroy = osd_window_destroy;
    self->block = osd_window_block;
    self->glyph = osd_window_glyph;
    self->ingredient = osd_window_ingredient;
    self->text = osd_window_text;
    self->bitmap = osd_window_bitmap;
    self->rectangle = osd_window_rectangle;
    self->line = osd_window_line;
    self->label = osd_window_label;
    self->palette = osd_window_palette;
    self->find_glyph = osd_window_find_glyph;
    self->visible = osd_window_visible;
    self->set_visible = osd_window_set_visible;
    self->rect = osd_window_rect;
    self->set_rect = osd_window_set_rect;
    self->paint = osd_window_paint;
    self->alpha = osd_window_alpha;
    self->move_to = osd_window_move_to;
    self->set_window_proc = osd_window_set_window_proc;
    self->send_message = osd_window_send_message;
    self->ram = osd_window_ram;
    self->dump = osd_window_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);

    priv->hw = hw;
    priv->ingredient_count = hw->bitmap_count + hw->text_count +
                             hw->rectangle_count + hw->line_count +
                             hw->other_ingredient_count;

    {
        osd_block_hw *block_hw;
        priv->blocks = MALLOC_OBJECT_ARRAY(osd_block*, hw->block_count);
        block_hw = (osd_block_hw *)(self->ram(self) + (hw->blocks_addr));
        for (i = 0; i < hw->block_count; i ++) {
            priv->blocks[i] = osd_block_create(self, block_hw);
            OSD_OBJECT_DUMP(priv->blocks[i]);
            block_hw ++;
        }
    }

    {
        osd_ingredient_hw *ingredient_hw;
        priv->ingredients = MALLOC_OBJECT_ARRAY(osd_ingredient*, priv->ingredient_count);
        ingredient_hw = (osd_ingredient_hw *)(self->ram(self) + (hw->ingredients_addr));
        for (i = 0; i < priv->ingredient_count; i ++) {
            priv->ingredients[i] = osd_ingredient_create(self, ingredient_hw);
            OSD_OBJECT_DUMP(priv->ingredients[i]);
            ingredient_hw ++;
        }
    }

    {
        osd_palette_hw *palette_hw;
        priv->palettes = MALLOC_OBJECT_ARRAY(osd_palette*, hw->palette_count);
        palette_hw = (osd_palette_hw *)(self->ram(self) + (hw->palettes_addr));
        for (i = 0; i < hw->palette_count; i ++) {
            priv->palettes[i] = osd_palette_create(self, palette_hw);
            OSD_OBJECT_DUMP(priv->palettes[i]);
            palette_hw ++;
        }
    }

    {
        osd_glyph_hw *glyph_hw;
        priv->glyphs = MALLOC_OBJECT_ARRAY(osd_glyph*, hw->glyph_count);
        glyph_hw = (osd_glyph_hw *)(self->ram(self) + (hw->glyphs_addr));
        for (i = 0; i < hw->glyph_count; i ++) {
            priv->glyphs[i] = osd_glyph_create(self, glyph_hw);
            OSD_OBJECT_DUMP(priv->glyphs[i]);
            glyph_hw ++;
        }
    }

    OSD_OBJECT_DUMP(self);

    return self;
}