#ifndef _OSD_BITMAP_H
#define _OSD_BITMAP_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_bitmap_priv osd_bitmap_priv;

struct _osd_bitmap {
    osd_ingredient parent;
    osd_bitmap_priv *priv;
    void (*set_current)(osd_bitmap *self, u8 bitmap_index);
    u8 (*current)(osd_bitmap *self);
    u8 (*count)(osd_bitmap *self);
};

EXTERNC osd_bitmap *osd_bitmap_create(osd_window *window, osd_ingredient_hw *hw);

#endif