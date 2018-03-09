#ifndef _OSD_CHARACTER_H
#define _OSD_CHARACTER_H

#include "osd_types.h"

void osd_character_paint(osd_scene *scene, osd_window *window, osd_block *block,
                         osd_ingredient *ingredient,
                         u32 *window_line_buffer,
                         u32 y);
#endif