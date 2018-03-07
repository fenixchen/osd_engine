#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"


void osd_glyph_paint(osd_scene *scene, osd_window *window, osd_block *block,
                     osd_ingredient *ingredient,
                     u32 *window_line_buffer,
                     u32 y) {
    osd_glyph *glyph = &ingredient->data.glyph;
    if (y < glyph->height) {
        u32 width = OSD_MIN(glyph->width, window->width - block->x);
        u32 x;
        for (x = glyph->pitch * y; x < glyph->pitch * y + width; x ++) {
            u8 alpha = ingredient->ram_data[x];
            if (alpha != 0) {
                u32 color = osd_ingredient_get_color(scene, window,
                                                     ingredient,
                                                     glyph->color);
                u32 index = block->x + x - glyph->pitch * y;

                window_line_buffer[index] = osd_blend_pixel(window_line_buffer[index], color, alpha);
            }
        }
    }
}
