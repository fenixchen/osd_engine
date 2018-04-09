#ifndef _OSD_EVENT_H
#define _OSD_EVENT_H

#include "osd_types.h"

typedef enum _osd_event_type osd_event_type;

enum _osd_event_type {
    OSD_EVENT_TIMER,
    OSD_EVENT_KEYDOWN,
    OSD_EVENT_WINDOW_INIT,
    OSD_EVENT_WINDOW_ENTER,
    OSD_EVENT_WINDOW_LEAVE,
};

typedef enum _osd_key osd_key;
enum _osd_key {
    OSD_KEY_NONE,
    OSD_KEY_UP,
    OSD_KEY_DOWN,
    OSD_KEY_LEFT,
    OSD_KEY_RIGHT,
    OSD_KEY_ENTER,
    OSD_KEY_BACK,
    OSD_KEY_MENU,
};
typedef struct _osd_event_data osd_event_data;

struct _osd_event_data {
    union {
        struct {
            u32 tick;
        } timer;
        struct {
            osd_key key;
        } keydown;
        struct {
            osd_window *window;
        } window;
    } data;
};

#endif