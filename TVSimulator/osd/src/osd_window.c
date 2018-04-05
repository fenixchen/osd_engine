#include "tv_api.h"
#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_block.h"

struct _osd_window_priv {
    osd_window_hw *hw;
    osd_scene *scene;
    osd_block** blocks;
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
        u32 block_start_y, block_height;
        osd_block_hw *block = priv->blocks[i]->hw(priv->blocks[i]);
        if (!block->visible) {
            continue;
        }
        ingredient = priv->scene->ingredient(priv->scene, block->ingredient_index);
        block_start_y = block->y + ingredient->start_y(ingredient);
        block_height = ingredient->height(ingredient, self);
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
    u32 i;
    osd_window_hw *window;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    window = priv->hw;

    TV_LOG("window\n"
           "\tvisible:%d, alpha:%d, z_order:%d\n"
           "\tx:%d, y:%d, width:%d, height:%d\n"
           "\tblock_count:%d, block_addr:%#x\n",
           window->visible, window->alpha, window->z_order,
           window->x, window->y, window->width, window->height,
           window->block_count, (unsigned int)priv->blocks);

    for (i = 0; i < window->block_count; i ++) {
        osd_block_hw *block = priv->blocks[i]->hw(priv->blocks[i]);
        TV_LOG("\t\tblock[%d] visible:%d, ingredient:%d, x:%d, y:%d\n",
               i, block->visible, block->ingredient_index, block->x, block->y);
    }
}

static osd_block* osd_window_block(osd_window *self, u16 index) {
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

static void osd_window_destroy(osd_window *self) {
    u32 i;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    for (i = 0; i < priv->hw->block_count; i ++) {
        priv->blocks[i]->destroy(priv->blocks[i]);
    }
    FREE_OBJECT(priv->blocks);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

static void osd_window_move_to(osd_window *self, int x, int y) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->hw->x = x;
    priv->hw->y = y;
}

osd_window *osd_window_create(osd_scene *scene, osd_window_hw *hw) {
    u32 i;
    osd_block_hw *block_hw;
    osd_window_priv *priv;

    osd_window *self = MALLOC_OBJECT(osd_window);
    priv = MALLOC_OBJECT(osd_window_priv);
    priv->scene = scene;
    self->priv = priv;
    self->destroy = osd_window_destroy;
    self->visible = osd_window_visible;
    self->set_visible = osd_window_set_visible;
    self->rect = osd_window_rect;
    self->set_rect = osd_window_set_rect;
    self->paint = osd_window_paint;
    self->alpha = osd_window_alpha;
    self->move_to = osd_window_move_to;
    self->block = osd_window_block;
    self->dump = osd_window_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);

    priv->hw = hw;
    priv->blocks = MALLOC_OBJECT_ARRAY(osd_block*, hw->block_count);
    block_hw = (osd_block_hw *)(scene->ram(scene) + (hw->blocks_addr));
    for (i = 0; i < hw->block_count; i ++) {
        priv->blocks[i] = osd_block_create(scene, block_hw);
        block_hw ++;
    }
    return self;
}