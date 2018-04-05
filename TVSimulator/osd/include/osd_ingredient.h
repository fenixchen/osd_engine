#ifndef _OSD_INGREDIENT_H
#define _OSD_INGREDIENT_H

#include "osd_types.h"

typedef struct _osd_ingredient_priv osd_ingredient_priv;

struct _osd_ingredient {
    osd_ingredient_priv *priv;
    void (*destroy)(osd_ingredient *self);
    u32 (*color)(osd_ingredient *self, u32 index);
    u32 (*color2)(osd_ingredient *self, u8 *color_ram, u32 index);
    u32 (*start_y)(osd_ingredient *self);
    u32 (*height)(osd_ingredient *self, osd_window *window);
    void (*paint)(osd_ingredient *self, osd_window *window, osd_block *block,
                  u32 *window_line_buffer, u32 y);
    u32 (*palette_index)(osd_ingredient *self);
    u8 (*type)(osd_ingredient *self);
    void (*dump)(osd_ingredient *self);
};

EXTERNC osd_ingredient *osd_ingredient_create(osd_scene *scene, osd_ingredient_hw *hw);

#endif