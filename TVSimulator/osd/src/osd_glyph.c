#include "tv_api.h"
#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_scene.h"
#include "osd_ingredient.h"
#include "osd_glyph.h"

struct _osd_glyph_priv {
    osd_glyph_hw *hw;
    osd_window *window;
};

static void osd_glyph_dump(osd_glyph *self) {
    TV_TYPE_GET_PRIV(osd_glyph_priv, self, priv);
    TV_LOG("Glyph\n\tchar:%d, font_id:%d, font_size:%d\n",
           priv->hw->char_code, priv->hw->font_id, priv->hw->font_size);
}

static void osd_glyph_destroy(osd_glyph *self) {
    TV_TYPE_GET_PRIV(osd_glyph_priv, self, priv);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_glyph_hw* osd_glyph_get_hw(osd_glyph *self) {
    TV_TYPE_GET_PRIV(osd_glyph_priv, self, priv);
    return priv->hw;
}

osd_glyph *osd_glyph_create(osd_window *window, osd_glyph_hw *hw) {
    osd_glyph_priv *priv;
    osd_glyph *self = MALLOC_OBJECT(osd_glyph);
    priv = MALLOC_OBJECT(osd_glyph_priv);
    priv->window = window;
    self->priv = priv;
    self->destroy = osd_glyph_destroy;
    self->hw = osd_glyph_get_hw;
    self->dump = osd_glyph_dump;
    TV_TYPE_FP_CHECK(self->destroy, self->dump);
    priv->hw = hw;
    return self;
}