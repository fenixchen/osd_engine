#ifndef _OSD_WINDOW_H
#define _OSD_WINDOW_H

#include "osd_types.h"

typedef struct _osd_window_priv osd_window_priv;

struct _osd_window {
    osd_window_priv *priv;
    void		(*destroy)(osd_window *self);
    int			(*visible)(osd_window *self);
    void		(*set_visible)(osd_window *self, int visible);
    int			(*paint)(osd_window *self, u32 *window_line_buffer, u16 y);
    osd_rect	(*rect)(osd_window *self);
    void		(*set_rect)(osd_window *self, osd_rect *rect);
    u8			(*alpha)(osd_window *self);
    u8			(*palette_index)(osd_window *self);
    void		(*move_to)(osd_window *self, int x, int y);
    void		(*dump)(osd_window *self);
};

osd_window *osd_window_create(osd_scene *scene, osd_window_hw *hw);

#endif