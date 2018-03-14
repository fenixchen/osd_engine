#ifndef _OSD_SCENE_H
#define _OSD_SCENE_H

#include "osd_types.h"

typedef void (*fn_set_pixel)(void *arg, int x, int y, u32 color);

typedef struct _osd_scene_priv osd_scene_priv;

struct _osd_scene {
    osd_scene_priv *priv;
    void (*destroy)(osd_scene *self);
    void (*paint)(osd_scene *self, fn_set_pixel set_pixel, void *arg, u32 *framebuffer);
    int (*timer)(osd_scene *self);
    osd_ingredient* (*ingredient)(osd_scene *self, u32 index);
    osd_palette* (*palette)(osd_scene *self, u32 index);
    const char * (*title)(osd_scene *self);
    u16 (*timer_ms)(osd_scene *self);
    osd_rect (*rect)(osd_scene *self);
    u8* (*ram)(osd_scene *self);
    void (*dump)(osd_scene *self);
};

EXTERNC osd_scene *osd_scene_create(const char *osd_file);

#endif