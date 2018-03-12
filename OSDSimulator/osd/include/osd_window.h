#ifndef _OSD_WINDOW_H
#define _OSD_WINDOW_H

#include "osd_types.h"

typedef struct _osd_window_priv osd_window_priv;

struct _osd_window {
    osd_window_priv *priv;
    void (*destroy)(osd_window *self);
    int (*is_visible)(osd_window *self);
    int (*paint)(osd_window *self,
                 osd_scene *scene,
                 u32 *window_line_buffer,
                 u16 y);
    osd_rect (*get_rect)(osd_window *self);
    u8 (*get_alpha)(osd_window *self);
    u8 (*get_palette_index)(osd_window *self);
    void (*dump)(osd_window *window);
};

osd_window *osd_window_create(osd_window_hw *hw, u8 *ram, u32 ram_base_addr);

#endif