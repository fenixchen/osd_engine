#ifndef _OSD_PROC_SYSSET_H_
#define _OSD_PROC_SYSSET_H_

#include "osd_event.h"
#include "osd_window.h"


enum center_window {
    CENTER_WINDOW_PICTURE_MODE  = 0,
    CENTER_WINDOW_SOUND_MODE,
    CENTER_WINDOW_CHANNEL,
    CENTER_WINDOW_FEATURE,
    CENTER_WINDOW_SETUP,
    CENTER_WINDOW_MAX
};

typedef struct _osd_proc_system_settings_priv osd_proc_system_settings_priv;
struct _osd_proc_system_settings_priv {
    osd_scene *scene;
    osd_window *window_left;
    osd_window *window_picture_mode;
    osd_window *window_sound_mode;
    osd_window *window_channel;
    osd_window *window_feature;
    osd_window *window_setup;
    osd_window *window_center[CENTER_WINDOW_MAX];
};

typedef struct _window_left_data window_left_data;
extern window_left_data left_data;
int osd_window_left_proc(osd_window_proc *proc,osd_event_type type,
                         osd_event_data *data);


typedef struct _window_picture_mode_data window_picture_mode_data;
extern window_picture_mode_data picture_mode_data;
int osd_window_picture_mode_proc(osd_window_proc *proc,osd_event_type type,
                                 osd_event_data *data);
#endif