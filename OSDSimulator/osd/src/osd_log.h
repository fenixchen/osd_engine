#ifndef _OSD_LOG_H
#define _OSD_LOG_H

#include "osd_types.h"


void log_global(osd_scene *scene);

void log_palette(int index, osd_palette *palette);

void log_ingredient(int index, osd_ingredient *ingredient);

void log_window(int index, osd_window *window);

#endif