#ifndef _OSD_PROC_H
#define _OSD_PROC_H

#include "osd_types.h"
#include "osd_event.h"

struct _osd_proc {
    void *priv;
    void (*destroy)(osd_proc *self);
    void (*init_ui)(osd_proc *self);
    int (*event)(osd_proc *self, osd_event_type type, osd_event_data *data);
};

#endif