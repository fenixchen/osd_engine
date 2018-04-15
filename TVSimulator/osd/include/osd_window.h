#ifndef _OSD_WINDOW_H
#define _OSD_WINDOW_H

#include "osd_types.h"
#include "osd_event.h"


typedef struct _osd_window_proc osd_window_proc;

typedef int (*osd_window_proc_handler)(osd_window_proc *proc, osd_event_type type, osd_event_data *data);

struct _osd_window_proc {
    u32 window_id;
    osd_proc *proc;
    osd_window *window;
    void *priv;
    osd_window_proc_handler handler;
};

typedef struct _osd_window_priv osd_window_priv;

struct _osd_window {
    osd_window_priv *priv;
    void		(*destroy)(osd_window *self);
    osd_block* (*block)(osd_window *self, u32 index);
    osd_glyph* (*glyph)(osd_window *self, u32 index);
    osd_ingredient* (*ingredient)(osd_window *self, u32 index);
    osd_text*	(*text)(osd_window *self, u32 index);
    osd_bitmap* (*bitmap)(osd_window *self, u32 index);
    osd_rectangle* (*rectangle)(osd_window *self, u32 index);
    osd_line* (*line)(osd_window *self, u32 index);
    osd_label* (*label)(osd_window *self, u32 index);
    osd_palette* (*palette)(osd_window *self, u32 index);
    u16			(*find_glyph)(osd_window *self, u16 char_code, u8 font_id, u8 font_size);
    int			(*visible)(osd_window *self);
    void		(*set_visible)(osd_window *self, int visible);
    osd_rect	(*rect)(osd_window *self);
    void		(*set_rect)(osd_window *self, osd_rect *rect);
    int			(*paint)(osd_window *self, u32 *window_line_buffer, u16 y);
    u8			(*alpha)(osd_window *self);
    void		(*move_to)(osd_window *self, int x, int y);
    void		(*set_window_proc)(osd_window *self, osd_window_proc *window_proc);
    int			(*send_message)(osd_window *window, osd_event_type type, osd_event_data *data);
    u8*			(*ram)(osd_window *self);
    void		(*dump)(osd_window *self);
};

osd_window *osd_window_create(osd_scene *scene, osd_window_hw *hw);

#define osd_window_show(window) (window)->set_visible(window, 1)

#define osd_window_hide(window) (window)->set_visible(window, 0)

#endif