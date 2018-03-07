#ifndef _OSD_WINDOW_H
#define _OSD_WINDOW_H

#include "osd_types.h"

int osd_window_paint(osd_scene *scene,
                     osd_window *window,
                     u32 *window_line_buffer,
                     u16 y);
#endif