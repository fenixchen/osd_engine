#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"


void osd_character_paint(osd_scene *scene, osd_window *window, osd_block *block,
                         osd_ingredient *ingredient,
                         u32 *window_line_buffer,
                         u32 y) {
    osd_character *character = &ingredient->data.character;
    osd_glyph *glyph = (osd_glyph *)ingredient->ram_data;
    if (y < glyph->height) {
        u32 width = OSD_MIN(glyph->width, window->width - block->x);
        u32 x;
        for (x = glyph->pitch * y; x < glyph->pitch * y + width; x ++) {
            u8 alpha = glyph->data[x];
            if (alpha != 0) {
                u32 color = osd_ingredient_get_color(scene, window,
                                                     ingredient,
                                                     character->color);
                u32 index = block->x + x - glyph->pitch * y;

                window_line_buffer[index] = osd_blend_pixel(window_line_buffer[index], color, alpha);
            }
        }
    }
}
