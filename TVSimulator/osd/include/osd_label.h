#ifndef _OSD_LABEL_H
#define _OSD_LABEL_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_label_priv osd_label_priv;

struct _osd_label {
    osd_ingredient parent;
    osd_label_priv *priv;
    void (*set_int)(osd_label *self, int value);
    void (*set_string)(osd_label *self, const t_wchar *str);
    void (*set_text)(osd_label *self, int text_index);
    int (*text)(osd_label *self);
    void (*set_state)(osd_label *self, int state);
    int (*state)(osd_label *self);
};

EXTERNC osd_label *osd_label_create(osd_window *window, osd_ingredient_hw *hw);

#endif