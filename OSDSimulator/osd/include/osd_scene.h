#ifndef _OSD_SCENE_H
#define _OSD_SCENE_H

#include "osd_types.h"

EXTERNC osd_scene *osd_scene_new(const char *target_folder);

EXTERNC void osd_scene_delete(osd_scene *scene);

typedef void (*fn_set_pixel)(void *arg, int x, int y, u32 color);

EXTERNC void osd_scene_paint(osd_scene *scene, u32 frame,
                             fn_set_pixel set_pixel, void *arg);

#endif