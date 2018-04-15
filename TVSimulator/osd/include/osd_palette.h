#ifndef _OSD_PALETTE_H
#define _OSD_PALETTE_H

#include "osd_types.h"

typedef struct _osd_palette_priv osd_palette_priv;

struct _osd_palette {
    osd_palette_priv *priv;
    void (*destroy)(osd_palette *self);
    u32 (*entry_count)(osd_palette *self);
    u8 (*pixel_bits)(osd_palette *self);
    u32 (*color)(osd_palette *self, u32 index);
    void (*dump)(osd_palette *self);
};

EXTERNC osd_palette *osd_palette_create(osd_window *window, osd_palette_hw *hw);

#endif