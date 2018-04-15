#ifndef _OSD_SCENE_H
#define _OSD_SCENE_H

#include "osd_types.h"
#include "osd_event.h"

typedef struct _osd_scene_priv osd_scene_priv;

struct _osd_scene {
    osd_scene_priv *priv;
    void (*destroy)(osd_scene *self);
    void (*paint)(osd_scene *self, u32 *framebuffer);
    osd_window* (*window)(osd_scene *self, u32 index);
    const char * (*title)(osd_scene *self);
    int (*timer_interval)(osd_scene *self);
    int (*trigger)(osd_scene *self, osd_event_type type, osd_event_data *data);
    osd_rect (*rect)(osd_scene *self);
    u8* (*ram)(osd_scene *self);
    void (*set_proc)(osd_scene *self, osd_proc *proc);
    void (*set_focused_window)(osd_scene *self, osd_window *window);
    osd_window* (*focused_window)(osd_scene *self);
    void (*dump)(osd_scene *self);
};

osd_scene *osd_scene_create(const char *osd_file, osd_proc *proc);

#define osd_scene_focus(scene, window) (scene)->set_focused_window(scene, window)

#endif