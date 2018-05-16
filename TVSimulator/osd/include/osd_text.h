#ifndef _OSD_TEXT_H
#define _OSD_TEXT_H

#include "osd_types.h"
#include "osd_ingredient.h"

typedef struct _osd_text_priv osd_text_priv;

struct _osd_text {
    osd_ingredient parent;
    osd_text_priv *priv;
    u8 (*font_id)(osd_text *self);
    u8 (*font_size)(osd_text *self);
    u16 (*length)(osd_text *self);
    void (*set_text)(osd_text *self, const wchar *str);
    void (*set_color)(osd_text *self, u32 color);
    u32 (*color)(osd_text *self);
};

EXTERNC osd_text *osd_text_create(osd_window *window, osd_ingredient_hw *hw);



#endif