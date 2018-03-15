#ifndef _OSD_EVENT_H
#define _OSD_EVENT_H

#include "osd_types.h"

typedef enum _osd_trigger_type osd_trigger_type;

enum _osd_trigger_type {
    OSD_TRIGGER_TIMER,
    OSD_TRIGGER_KEYDOWN,
};

typedef enum _osd_key osd_key;
enum _osd_key {
    OSD_KEY_NONE,
    OSD_KEY_UP,
    OSD_KEY_DOWN,
    OSD_KEY_LEFT,
    OSD_KEY_RIGHT,
    OSD_KEY_ENTER,
};
typedef struct _osd_trigger_data osd_trigger_data;

struct _osd_trigger_data {
    union {
        struct {
            u32 tick;
        } timer;
        struct {
            osd_key key;
        } keydown;
    } data;
};

#endif