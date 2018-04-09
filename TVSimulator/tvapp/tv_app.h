#ifndef _TV_APP_H
#define _TV_APP_H

#include "osd_types.h"
#include "osd_event.h"


typedef struct _tv_app_priv tv_app_priv;
struct _tv_app {
    tv_app_priv *priv;
    void (*destroy)(tv_app *self);
    void (*paint)(tv_app *self, u32 *framebuffer);
    int (*trigger)(tv_app *self, osd_event_type type, osd_event_data *data);
    osd_scene* (*active_scene)(tv_app *self);
    osd_rect (*rect)(tv_app *self);
    const char* (*title)(tv_app *self);
    int (*timer_interval)(tv_app *self);
    int (*load)(tv_app *self, const char *osd_file);
};

tv_app* tv_app_create(void);

#endif