#ifndef _OSD_EVENT_H
#define _OSD_EVENT_H

#include "osd_types.h"

typedef enum _osd_trigger_type osd_trigger_type;

enum _osd_trigger_type {
    OSD_TRIGGER_TIMER,
    OSD_TRIGGER_KEYDOWN,
};

typedef struct _osd_trigger_data osd_trigger_data;

struct _osd_trigger_data {
    union {
        struct {
            u32 tick;
        } timer;
        struct {
            u32 key;
        } keydown;
    } data;
};

#endif