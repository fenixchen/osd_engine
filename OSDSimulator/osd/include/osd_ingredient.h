#ifndef _OSD_INGREDIENT_H
#define _OSD_INGREDIENT_H

#include "osd_types.h"

u32 osd_ingredient_get_color(osd_scene *scene, osd_window *window,
                             osd_ingredient *ingredient,
                             u32 index);

u32 osd_ingredient_get_color2(osd_scene *scene, osd_window *window,
                              osd_ingredient *ingredient,
                              u8 *color_ram,
                              u32 index);

u32 osd_ingredient_start_y(osd_ingredient *ingredient);

u32 osd_ingredient_height(osd_ingredient *ingredient, osd_window *window);

void osd_ingredient_paint(osd_scene *scene, osd_window *window, osd_block *block,
                          osd_ingredient *ingredient,
                          u32 *window_line_buffer,
                          u32 y);

#endif