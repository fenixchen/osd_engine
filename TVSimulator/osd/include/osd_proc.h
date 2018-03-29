#ifndef _OSD_PROC_H
#define _OSD_PROC_H

#include "osd_types.h"
#include "osd_event.h"

struct _osd_proc {
    void *priv;
    void (*destroy)(osd_proc *self);
    int (*event)(osd_proc *self, osd_trigger_type type, osd_trigger_data *data);
};

#endif