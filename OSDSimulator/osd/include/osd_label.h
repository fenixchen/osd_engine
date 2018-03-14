#ifndef _OSD_LABEL_H
#define _OSD_LABEL_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_label_priv osd_label_priv;

struct _osd_label {
    osd_ingredient parent;
    osd_label_priv *priv;
    void (*set_value)(osd_label *self, int value);
};

EXTERNC osd_label *osd_label_create(osd_scene *scene, osd_ingredient_hw *hw);

#endif