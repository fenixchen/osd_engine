#ifndef _OSD_CHARACTER_H
#define _OSD_CHARACTER_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_character_priv osd_character_priv;

struct _osd_character {
    osd_ingredient parent;
    osd_character_priv *priv;
    void (*set_glyph)(osd_character *self, u32 glyph_index);
    osd_glyph* (*glyph)(osd_character *self);
};

EXTERNC osd_character *osd_character_create(osd_scene *scene, osd_ingredient_hw *hw);



#endif