#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_rectangle.h"

static int osd_rectangle_border_paint(osd_scene *scene,
                                      osd_window *window,
                                      osd_block *block,
                                      osd_ingredient *ingredient,
                                      u32 *window_line_buffer,
                                      u32 y) {
    u32 x, color, margin;
    osd_rectangle *rect = &ingredient->data.rect;
    u16 width = rect->width == 0xFFFF ? window->width : rect->width;
    u16 height = rect->height == 0xFFFF ? window->height : rect->height;
    if (y < rect->border_weight) {
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_top);
        margin = rect->border_weight - (rect->border_weight - y);
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            if (osd_line_style_check(rect->border_style, x))
                window_line_buffer[x] = color;
        }
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + y; x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - y; x < (u32)(block->x + width); x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        return 1;
    }

    if (y >= (u32)(height - rect->border_weight)) {
        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_bottom);
        margin = height - y;
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            if (osd_line_style_check(rect->border_style, x))
                window_line_buffer[x] = color;
        }

        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + height - y; x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }

        color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
        for (x = block->x + width - height + y; x < (u32)(block->x + width); x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        return 1;
    }

    color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_left);
    for (x = block->x; x < (u32)(block->x + rect->border_weight); x ++) {
        window_line_buffer[x] = color;
    }

    color = osd_ingredient_get_color(scene, window, ingredient, rect->border_color_right);
    for (x = block->x + width - rect->border_weight; x < (u32)(block->x + width); x ++) {
        window_line_buffer[x] = color;
    }

    return 0;
}



static void osd_rectangle_backgroud_paint(osd_scene *scene, osd_window *window, osd_block *block,
        osd_ingredient *ingredient,
        u32 *window_line_buffer,
        u32 y) {
    u32 x, bg_color_start, bg_color_end, color, color_steps;
    double r_delta, g_delta, b_delta;

    osd_rectangle *rect = &ingredient->data.rect;
    u16 width = rect->width == 0xFFFF ? window->width : rect->width;
    u16 height = rect->height == 0xFFFF ? window->height : rect->height;
    if (rect->bgcolor_start == 0) {
        return;
    }
    bg_color_start = osd_ingredient_get_color(scene, window, ingredient, rect->bgcolor_start);
    bg_color_end = osd_ingredient_get_color(scene, window, ingredient, rect->bgcolor_end);

    switch (rect->gradient_mode) {
    case OSD_GRADIENT_MODE_NONE:
        return;
    case OSD_GRADIENT_MODE_SOLID:
        color_steps = 1;
        break;
    case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
        color_steps = width - rect->border_weight * 2;
        break;
    case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
        color_steps = height - rect->border_weight * 2;
        break;
    case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
    case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
        color_steps = (width - rect->border_weight * 2) * height - rect->border_weight * 2;
        break;
    default:
        assert(0);
    }
    r_delta = (OSD_R(bg_color_end) - OSD_R(bg_color_start)) / (double)color_steps;
    g_delta = (OSD_G(bg_color_end) - OSD_G(bg_color_start)) / (double)color_steps;
    b_delta = (OSD_B(bg_color_end) - OSD_B(bg_color_start)) / (double)color_steps;

    color = bg_color_start;
    for (x = block->x + rect->border_weight;
            x < (u32)(block->x + width - rect->border_weight); ++ x) {
        u32 factor;
        switch (rect->gradient_mode) {
        case OSD_GRADIENT_MODE_SOLID:
            factor = 0;
            break;
        case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
            factor = x - (block->x + rect->border_weight);
            break;
        case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
            factor = y;
            break;
        case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
            factor = (x - (block->x + rect->border_weight)) * y;
            break;
        case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
            factor = (x - (block->x + rect->border_weight)) * (rect->height - rect->border_weight - y);
            break;
        default:
            assert(0);
        }
        color = osd_color_add(bg_color_start,
                              (int)r_delta * factor,
                              (int)g_delta * factor,
                              (int)b_delta * factor);
        window_line_buffer[x] = color;
    }
}

void osd_rectangle_paint(osd_scene *scene, osd_window *window, osd_block *block,
                         osd_ingredient *ingredient,
                         u32 *window_line_buffer,
                         u32 y) {
    int is_border = 0;
    osd_rectangle *rect = &ingredient->data.rect;
    if (rect->border_weight > 0) {
        is_border = osd_rectangle_border_paint(scene, window, block, ingredient, window_line_buffer, y);
    }
    if (!is_border && rect->gradient_mode != 0) {
        osd_rectangle_backgroud_paint(scene, window, block, ingredient, window_line_buffer, y);
    }
}