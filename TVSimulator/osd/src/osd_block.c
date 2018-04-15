#include "tv_api.h"
#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_block.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_block.h"

struct _osd_block_priv {
    osd_block_hw *hw;
    osd_window *window;
};

static void osd_block_dump(osd_block *self) {
    osd_block_hw *block;
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    block = priv->hw;
    TV_LOG("Block\n"
           "\tblock visible:%d, ingredient:%d, x:%d, y:%d, (%d x %d)\n",
           block->visible, block->ingredient_index, block->x, block->y, block->width, block->height);
}

static u16 osd_block_ingredient_index(osd_block *self) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    return priv->hw->ingredient_index;
}

static int osd_block_visible(osd_block *self) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    return priv->hw->visible;
}

static void osd_block_set_visible(osd_block *self, int visible) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    priv->hw->visible = visible;
}

static void osd_block_destroy(osd_block *self) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_block_hw* osd_block_get_hw(osd_block *self) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    return priv->hw;
}

static osd_point osd_block_position(osd_block *self) {
    osd_point point;
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    point.x = priv->hw->x;
    point.y = priv->hw->y;
    return point;
}

static void	osd_block_set_position(osd_block *self, osd_point *point) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    TV_ASSERT(point);
    priv->hw->x = point->x;
    priv->hw->y = point->y;
}


static void osd_block_move_to(osd_block *self, int x, int y) {
    TV_TYPE_GET_PRIV(osd_block_priv, self, priv);
    priv->hw->x = x;
    priv->hw->y = y;
}

osd_block *osd_block_create(osd_window *window, osd_block_hw *hw) {
    osd_block_priv *priv;
    osd_block *self = MALLOC_OBJECT(osd_block);
    priv = MALLOC_OBJECT(osd_block_priv);
    priv->window = window;
    self->priv = priv;
    self->destroy = osd_block_destroy;
    self->ingredient_index = osd_block_ingredient_index;
    self->visible = osd_block_visible;
    self->set_visible = osd_block_set_visible;
    self->position = osd_block_position;
    self->set_position = osd_block_set_position;
    self->hw = osd_block_get_hw;
    self->dump = osd_block_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);
    priv->hw = hw;
    return self;
}