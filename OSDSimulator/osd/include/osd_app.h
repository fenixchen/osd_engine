#ifndef _OSD_APP_H
#define _OSD_APP_H

#include "osd_types.h"
#include "osd_event.h"

struct _osd_app {
    void *priv;
    void (*destroy)(osd_app *self);
    int (*event)(osd_app *self, osd_trigger_type type, osd_trigger_data *data);
};

#endif