#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"

void osd_bitmap_paint(osd_scene *scene, osd_window *window, osd_block *block,
                      osd_ingredient *ingredient,
                      u32 *window_line_buffer,
                      u32 y) {
    osd_bitmap *bitmap = &ingredient->data.bitmap;
    if (y < bitmap->height) {
        u32 width = OSD_MIN(bitmap->width, window->width - block->x);
        u32 start = ingredient->current_bitmap * bitmap->width * bitmap->height +
                    bitmap->width * y;
        u32 x;
        for (x = start; x < start + width; x ++) {
            u32 color = osd_ingredient_get_color2(scene, window,
                                                  ingredient,
                                                  ingredient->ram_data,
                                                  x);
            window_line_buffer[block->x + x - start] = color;
        }
    }
}