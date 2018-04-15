#ifndef _OSD_LINE_H
#define _OSD_LINE_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_line_priv osd_line_priv;

struct _osd_line {
    osd_ingredient parent;
    osd_line_priv *priv;
};

EXTERNC osd_line *osd_line_create(osd_window *window, osd_ingredient_hw *hw);

#endif