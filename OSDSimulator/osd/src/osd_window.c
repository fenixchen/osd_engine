#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_window.h"

int osd_window_paint(osd_scene *scene,
                     osd_window *window,
                     u32 *window_line_buffer,
                     u16 y) {
    u32 i;
    u16 window_y;
    int paint = 0;
    window_y = y - window->y;

    for (i = 0; i < window->block_count; i ++) {
        osd_ingredient *ingredient;
        u32 block_start_y, block_height;
        osd_block *block = &window->blocks[i];
        if (!block->visible) {
            continue;
        }
        ingredient = scene->ingredients[block->ingredient_index];
        assert(ingredient);
        block_start_y = block->y + osd_ingredient_start_y(ingredient);
        block_height = osd_ingredient_height(ingredient, window);
        if (block_start_y <= window_y && window_y < block_start_y + block_height) {
            osd_ingredient_paint(scene, window, block, ingredient,
                                 window_line_buffer,
                                 window_y - block_start_y);
            ++ paint;
        }
    }
    return paint;
}
