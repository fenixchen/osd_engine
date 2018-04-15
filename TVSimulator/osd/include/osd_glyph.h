#ifndef _OSD_GLYPH_H
#define _OSD_GLYPH_H

#include "osd_types.h"

typedef struct _osd_glyph_priv osd_glyph_priv;

struct _osd_glyph {
    osd_glyph_priv *priv;
    void		  (*destroy)(osd_glyph *self);
    osd_glyph_hw* (*hw)(osd_glyph *self);
    void		  (*dump)(osd_glyph *self);
};

osd_glyph *osd_glyph_create(osd_window *window, osd_glyph_hw *hw);

#endif