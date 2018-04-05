#ifndef _OSD_SCENE_H
#define _OSD_SCENE_H

#include "osd_types.h"
#include "osd_event.h"

typedef struct _osd_scene_priv osd_scene_priv;

struct _osd_scene {
    osd_scene_priv *priv;
    void (*destroy)(osd_scene *self);
    void (*paint)(osd_scene *self, u32 *framebuffer);
    osd_block* (*block)(osd_scene *self, u32 index);
    osd_ingredient* (*ingredient)(osd_scene *self, u32 index);
    osd_character* (*character)(osd_scene *self, u32 index);
    osd_bitmap* (*bitmap)(osd_scene *self, u32 index);
    osd_rectangle* (*rectangle)(osd_scene *self, u32 index);
    osd_line* (*line)(osd_scene *self, u32 index);
    osd_label* (*label)(osd_scene *self, u32 index);
    osd_palette* (*palette)(osd_scene *self, u32 index);
    osd_window* (*window)(osd_scene *self, u32 index);
    const char * (*title)(osd_scene *self);
    int (*timer_interval)(osd_scene *self);
    int (*trigger)(osd_scene *self, osd_trigger_type type, osd_trigger_data *data);
    osd_rect (*rect)(osd_scene *self);
    u8* (*ram)(osd_scene *self);
    u32 (*glyph_addr)(osd_scene *self, u16 index);
    u16 (*find_glyph)(osd_scene *self, u16 char_code, u8 font_id, u8 font_size);
    void (*set_proc)(osd_scene *self, osd_proc *proc);
    void (*dump)(osd_scene *self);
};

EXTERNC osd_scene *osd_scene_create(const char *osd_file, osd_proc *proc);

#endif