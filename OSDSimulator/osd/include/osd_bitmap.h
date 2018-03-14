#ifndef _OSD_BITMAP_H
#define _OSD_BITMAP_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_bitmap_priv osd_bitmap_priv;

struct _osd_bitmap {
    osd_ingredient parent;
    osd_bitmap_priv *priv;
};

EXTERNC osd_bitmap *osd_bitmap_create(osd_scene *scene, osd_ingredient_hw *hw);

#endif