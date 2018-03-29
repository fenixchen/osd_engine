#ifndef _OSD_RECTANGLE_H
#define _OSD_RECTANGLE_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_rectangle_priv osd_rectangle_priv;

struct _osd_rectangle {
    osd_ingredient parent;
    osd_rectangle_priv *priv;
    osd_rect (*rect)(osd_rectangle *self);
    void (*set_rect)(osd_rectangle *self, osd_rect *rect);
};

EXTERNC osd_rectangle *osd_rectangle_create(osd_scene *scene, osd_ingredient_hw *hw);

#endif