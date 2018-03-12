#include "tv_types.h"
#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"

struct _osd_window_priv {
    osd_window_hw *hw;
    osd_block* blocks;
};

static int osd_window_paint(osd_window *self,
                            osd_scene *scene,
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
        osd_block *block = &priv->blocks[i];
        if (!block->visible) {
            continue;
        }
        ingredient = scene->ingredients[block->ingredient_index];
        TV_ASSERT(ingredient);
        block_start_y = block->y + osd_ingredient_start_y(ingredient);
        block_height = osd_ingredient_height(ingredient, self);
        if (block_start_y <= window_y && window_y < block_start_y + block_height) {
            osd_ingredient_paint(scene, self, block, ingredient,
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

    OSD_LOG("window\n"
            "\tpalette:%d, visible:%d, alpha:%d, z_order:%d\n"
            "\tx:%d, y:%d, width:%d, height:%d\n"
            "\tblock_count:%d, block_addr:%#x\n",
            window->palette_index, window->visible, window->alpha, window->z_order,
            window->x, window->y, window->width, window->height,
            window->block_count, (unsigned int)priv->blocks);

    for (i = 0; i < window->block_count; i ++) {
        osd_block *block = &priv->blocks[i];
        OSD_LOG("\t\tblock[%d] visible:%d, ingredient:%d, x:%d, y:%d\n",
                i, block->visible, block->ingredient_index, block->x, block->y);
    }
}

static osd_rect osd_window_get_rect(osd_window *self) {
    osd_rect rect;
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    rect.x = priv->hw->x;
    rect.y = priv->hw->y;
    rect.width = priv->hw->width;
    rect.height = priv->hw->height;
    return rect;
}

static u8 osd_window_get_alpha(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->hw->alpha;
}

static int osd_window_is_visible(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->hw->visible;
}

static u8 osd_window_get_palette_index(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    return priv->hw->palette_index;
}

static void osd_window_destroy(osd_window *self) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    FREE_OBJECT(priv->blocks);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

static void osd_window_move_to(osd_window *self, int x, int y) {
    TV_TYPE_GET_PRIV(osd_window_priv, self, priv);
    priv->hw->x = x;
    priv->hw->y = y;
}

osd_window *osd_window_create(osd_window_hw *hw, u8 *ram, u32 ram_base_addr) {
    u32 i;
    osd_block *block;
    osd_window_priv *priv;

    osd_window *self = MALLOC_OBJECT(osd_window);
    priv = MALLOC_OBJECT(osd_window_priv);
    self->priv = priv;
    self->destroy = osd_window_destroy;
    self->is_visible = osd_window_is_visible;
    self->get_rect = osd_window_get_rect;
    self->paint = osd_window_paint;
    self->get_palette_index = osd_window_get_palette_index;
    self->get_alpha = osd_window_get_alpha;
    self->move_to = osd_window_move_to;
    self->dump = osd_window_dump;

    priv->hw = hw;
    priv->blocks = MALLOC_OBJECT_ARRAY(osd_block, hw->block_count);
    block = (osd_block *)(ram + (hw->blocks_addr - ram_base_addr));
    for (i = 0; i < hw->block_count; i ++) {
        memcpy(&priv->blocks[i], block, sizeof(osd_block));
        block ++;
    }
    return self;
}