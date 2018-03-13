#include "osd_types.h"
#include "osd_palette.h"
#include "osd_scene.h"

struct _osd_palette_priv {
    osd_palette_hw *hw;
    osd_scene *scene;
    u32 *luts;
};

static void osd_palette_destroy(osd_palette *self) {
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static u32 osd_palette_color(osd_palette *self, u32 index) {
    TV_TYPE_GET_PRIV(osd_palette_priv, self, priv);
    return priv->luts[index];
}

static u8 osd_palette_pixel_bits(osd_palette *self) {
    TV_TYPE_GET_PRIV(osd_palette_priv, self, priv);
    return priv->hw->pixel_bits;
}

static u32 osd_palette_entry_count(osd_palette *self) {
    TV_TYPE_GET_PRIV(osd_palette_priv, self, priv);
    return priv->hw->entry_count;
}


static void osd_palette_dump(osd_palette *self) {
    TV_TYPE_GET_PRIV(osd_palette_priv, self, palette);
    OSD_LOG("palette\n\tpixel_format:%d, pixel_bits:%d, "
            "entry_count:%d, luts_addr:%#x\n",
            palette->hw->pixel_format, palette->hw->pixel_bits,
            palette->hw->entry_count,
            palette->hw->luts_addr);
}

osd_palette *osd_palette_create(osd_scene *scene, osd_palette_hw *hw) {
    osd_palette_priv *priv;

    osd_palette *self = MALLOC_OBJECT(osd_palette);
    priv = MALLOC_OBJECT(osd_palette_priv);
    self->priv = priv;
    priv->scene = scene;
    priv->hw = hw;

    self->destroy = osd_palette_destroy;
    self->color = osd_palette_color;
    self->pixel_bits = osd_palette_pixel_bits;
    self->dump = osd_palette_dump;
    self->entry_count = osd_palette_entry_count;

    priv->luts = (u32*)(scene->ram(scene) + priv->hw->luts_addr);
    return self;
}