#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"


void osd_character_paint(osd_scene *scene, osd_window *window, osd_block *block,
                         osd_ingredient *ingredient,
                         u32 *window_line_buffer,
                         u32 y) {
    u32 x, width, offset, col;
    u32 color;
    osd_character *character = &ingredient->data.character;
    osd_glyph *glyph = (osd_glyph *)ingredient->ram_data;
    if (y >= glyph->height)  return;

    color = osd_ingredient_get_color(scene, window,
                                     ingredient,
                                     character->color);
    width = OSD_MIN(glyph->width, window->width - block->x);
    offset = glyph->pitch * y;
    col = block->x + glyph->left;
    for (x = 0; x < width; x ++) {
        if (glyph->monochrome) {
            u8 pixel = glyph->data[offset + (x >> 3)];
            pixel &= (128 >> (x & 7));
            if (pixel) {
                window_line_buffer[col] = color;
            }
        } else {
            u8 alpha = glyph->data[x];
            if (alpha != 0) {
                window_line_buffer[col] = osd_blend_pixel(window_line_buffer[col], color, alpha);
            }
        }
        col ++;
    }
}
