#ifndef _OSD_RECTANGLE_H
#define _OSD_RECTANGLE_H

#include "osd_types.h"

void osd_rectangle_paint(osd_scene *scene, osd_window *window, osd_block *block,
                         osd_ingredient *ingredient,
                         u32 *window_line_buffer,
                         u32 y);
#endif