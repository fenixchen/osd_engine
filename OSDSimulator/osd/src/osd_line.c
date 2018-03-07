#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"

void osd_line_paint(osd_scene *scene, osd_window *window, osd_block *block,
                    osd_ingredient *ingredient,
                    u32 *window_line_buffer,
                    u32 y) {
    u32 x;
    osd_line *line = &ingredient->data.line;
    u32 color = osd_ingredient_get_color(scene, window, ingredient, line->color);
    u32 x1 = line->x1;
    u32 x2 = line->x2;
    u32 y1 = line->y1;
    u32 y2 = line->y2;
    if (y1 == y2) {
        if (y1 <= y && y < y1 + line->weight) {
            for (x = block->x + x1; x < block->x + x2; x ++) {
                if (osd_line_style_check(line->style, x)) {
                    window_line_buffer[x] = color;
                }
            }
        }
    } else if (x1 == x2) {
        for (x = 0; x < line->weight; x ++) {
            if (osd_line_style_check(line->style, y)) {
                window_line_buffer[block->x + x1 + x] = color;
            }
        }
    } else {
        double slope = (double)((int)x2 - (int)x1) / (double)((int)y2 - (int)y1);
        u32 px = x1 + (u32)(slope * (y - y1));
        if (osd_line_style_check(line->style, y)) {
            for (x = 0; x < line->weight; x++) {
                assert(block->x + px + x <= window->width);
                window_line_buffer[block->x + px + x] = color;
            }
        }
    }
}