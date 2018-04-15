#ifndef _OSD_RECTANGLE_H
#define _OSD_RECTANGLE_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_rectangle_priv osd_rectangle_priv;

struct _osd_rectangle {
    osd_ingredient parent;
    osd_rectangle_priv *priv;
};

EXTERNC osd_rectangle *osd_rectangle_create(osd_window *window, osd_ingredient_hw *hw);

#endif