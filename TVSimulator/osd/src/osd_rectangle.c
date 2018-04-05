#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_rectangle.h"
#include "osd_window.h"


struct _osd_rectangle_priv {
    osd_rectangle_hw *rectangle;
};

static int osd_rectangle_border_paint(osd_rectangle *self,
                                      osd_window *window,
                                      osd_block_hw *block,
                                      u32 *window_line_buffer,
                                      u32 y) {
    u32 x, color, margin;
    osd_rect window_rect;
    osd_rectangle_hw *rect;
    u16 width, height;
    osd_ingredient *ingredient = (osd_ingredient *)self;
    TV_TYPE_GET_PRIV(osd_rectangle_priv, self, priv);
    rect = priv->rectangle;
    window_rect = window->rect(window);
    width = rect->width == 0xFFFF ? window_rect.width : rect->width;
    height = rect->height == 0xFFFF ?window_rect.height : rect->height;

    if (y < rect->border_weight) {
        color = ingredient->color(ingredient, rect->border_color_top);
        margin = rect->border_weight - (rect->border_weight - y);
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            if (osd_line_style_check(rect->border_style, x))
                window_line_buffer[x] = color;
        }
        color = ingredient->color(ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + y; x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        color = ingredient->color(ingredient, rect->border_color_right);
        for (x = block->x + width - y; x < (u32)(block->x + width); x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        return 1;
    }

    if (y >= (u32)(height - rect->border_weight)) {
        color = ingredient->color(ingredient, rect->border_color_bottom);
        margin = height - y;
        for (x = block->x + margin; x < block->x + width - margin; x ++) {
            if (osd_line_style_check(rect->border_style, x))
                window_line_buffer[x] = color;
        }

        color = ingredient->color(ingredient, rect->border_color_left);
        for (x = block->x; x < block->x + height - y; x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }

        color = ingredient->color(ingredient, rect->border_color_right);
        for (x = block->x + width - height + y; x < (u32)(block->x + width); x ++) {
            if (osd_line_style_check(rect->border_style, y))
                window_line_buffer[x] = color;
        }
        return 1;
    }

    color = ingredient->color(ingredient, rect->border_color_left);
    for (x = block->x; x < (u32)(block->x + rect->border_weight); x ++) {
        window_line_buffer[x] = color;
    }

    color = ingredient->color(ingredient, rect->border_color_right);
    for (x = block->x + width - rect->border_weight; x < (u32)(block->x + width); x ++) {
        window_line_buffer[x] = color;
    }

    return 0;
}



static void osd_rectangle_backgroud_paint(osd_rectangle *self,
        osd_window *window,
        osd_block_hw *block,
        u32 *window_line_buffer,
        u32 y) {
    u32 x, bg_color_start, bg_color_end, color, color_steps;
    double r_delta, g_delta, b_delta;
    osd_rect window_rect;
    u16 width, height, fill_width, fill_height;
    osd_rectangle_hw *rect;
    u32 factor_x, factor_y;

    osd_ingredient *ingredient = (osd_ingredient *)self;
    TV_TYPE_GET_PRIV(osd_rectangle_priv, self, priv);
    rect = priv->rectangle;

    window_rect = window->rect(window);

    width = rect->width == 0xFFFF ? window_rect.width : rect->width;
    height = rect->height == 0xFFFF ?window_rect.height : rect->height;

    if (rect->bgcolor_start == 0) {
        return;
    }
    bg_color_start = ingredient->color(ingredient, rect->bgcolor_start);
    bg_color_end = ingredient->color(ingredient, rect->bgcolor_end);

    fill_width = width - rect->border_weight * 2;
    fill_height = height - rect->border_weight * 2;

    switch (rect->gradient_mode) {
    case OSD_GRADIENT_MODE_NONE:
        return;
    case OSD_GRADIENT_MODE_SOLID:
        color_steps = 1;
        break;
    case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
        color_steps = fill_width;
        break;
    case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
        color_steps = fill_height;
        break;
    case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
    case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
        color_steps = fill_width * fill_height;
        break;
    case OSD_GRADIENT_MODE_CORNER_TO_CENTER:
        color_steps = (fill_width * fill_height) >> 2;
        break;
    default:
        TV_ASSERT(0);
    }
    r_delta = (OSD_R(bg_color_end) - OSD_R(bg_color_start)) / (double)color_steps;
    g_delta = (OSD_G(bg_color_end) - OSD_G(bg_color_start)) / (double)color_steps;
    b_delta = (OSD_B(bg_color_end) - OSD_B(bg_color_start)) / (double)color_steps;

    color = bg_color_start;
    for (x = block->x + rect->border_weight;
            x < (u32)(block->x + width - rect->border_weight); ++ x) {
        u16 fill_x = x - (block->x + rect->border_weight);
        u16 fill_y = y - rect->border_weight;
        u32 factor;
        switch (rect->gradient_mode) {
        case OSD_GRADIENT_MODE_SOLID:
            factor = 0;
            break;
        case OSD_GRADIENT_MODE_LEFT_TO_RIGHT:
            factor = fill_x;
            break;
        case OSD_GRADIENT_MODE_TOP_TO_BOTTOM:
            factor = y;
            break;
        case OSD_GRADIENT_MODE_TOP_LEFT_TO_BOTTOM_RIGHT:
            factor = fill_x * fill_y;
            break;
        case OSD_GRADIENT_MODE_BOTTOM_LEFT_TO_TOP_RIGHT:
            factor = fill_x * (fill_height - fill_y);
            break;
        case OSD_GRADIENT_MODE_CORNER_TO_CENTER:
            if (fill_x <= (fill_width >> 1)) {
                factor_x = fill_x;
            } else {
                factor_x = fill_width - fill_x;
            }
            if (fill_y <= (fill_height >> 1)) {
                factor_y = fill_y;
            } else {
                factor_y = (fill_height - fill_y);
                factor = factor_x * factor_y;
            }
            factor = factor_x * factor_y;
            break;
        default:
            TV_ASSERT(0);
        }
        color = osd_color_add(bg_color_start,
                              (int)(r_delta * factor),
                              (int)(g_delta * factor),
                              (int)(b_delta * factor));
        window_line_buffer[x] = color;
    }
}

static void osd_rectangle_paint(osd_ingredient *self,
                                osd_window *window,
                                osd_block_hw *block,
                                u32 *window_line_buffer,
                                u32 y) {
    int is_border = 0;
    osd_rectangle *rectangle_self = (osd_rectangle *)self;
    TV_TYPE_GET_PRIV(osd_rectangle_priv, rectangle_self, priv);
    if (priv->rectangle->border_weight > 0) {
        is_border = osd_rectangle_border_paint(rectangle_self,
                                               window,
                                               block,
                                               window_line_buffer, y);
    }
    if (!is_border && priv->rectangle->gradient_mode != 0) {
        osd_rectangle_backgroud_paint(rectangle_self,
                                      window,
                                      block,
                                      window_line_buffer,
                                      y);
    }
}


static u32 osd_rectangle_start_y(osd_ingredient *self) {
    return 0;
}

static u32 osd_rectangle_height(osd_ingredient *self, osd_window *window) {
    osd_rectangle *rectangle_self = (osd_rectangle *)self;
    return rectangle_self->priv->rectangle->height;
}

static void osd_rectangle_dump(osd_ingredient *ingredient) {
    osd_rectangle_hw *rect;
    osd_rectangle *self = (osd_rectangle *)ingredient;
    TV_TYPE_GET_PRIV(osd_rectangle_priv, self, priv);

    rect = priv->rectangle;
    TV_LOG("Rectangle\n\tpalette:%d, gradient_mode:%d, width:%d, height:%d\n"
           "\tborder_color:[%d,%d,%d,%d], border_weight:%d, border_style:%d\n"
           "\tbgcolor:[%d,%d]\n",
           ingredient->palette_index(ingredient),
           rect->gradient_mode,
           rect->width, rect->height,
           rect->border_color_top, rect->border_color_bottom, rect->border_color_left,
           rect->border_color_right, rect->border_weight, rect->border_style,
           rect->bgcolor_start, rect->bgcolor_end);

}

osd_rect osd_rectangle_rect(osd_rectangle *self) {
    osd_rect rect;
    TV_TYPE_GET_PRIV(osd_rectangle_priv, self, priv);
    rect.x = rect.y = 0;
    rect.width = priv->rectangle->width;
    rect.height = priv->rectangle->height;
    return rect;
}
void osd_rectangle_set_rect(osd_rectangle *self, osd_rect *rect) {
    TV_TYPE_GET_PRIV(osd_rectangle_priv, self, priv);
    priv->rectangle->width = rect->width;
    priv->rectangle->height = rect->height;
}

static void osd_rectangle_destroy(osd_ingredient *self) {
    osd_rectangle *rectangle_self = (osd_rectangle *)self;
    FREE_OBJECT(rectangle_self->priv);
    FREE_OBJECT(self);
}

osd_rectangle *osd_rectangle_create(osd_scene *scene, osd_ingredient_hw *hw) {
    osd_rectangle *self = MALLOC_OBJECT(osd_rectangle);
    self->priv = MALLOC_OBJECT(osd_rectangle_priv);
    self->priv->rectangle = &hw->data.rectangle;

    self->parent.destroy = osd_rectangle_destroy;
    self->parent.paint = osd_rectangle_paint;
    self->parent.destroy = osd_rectangle_destroy;
    self->parent.start_y = osd_rectangle_start_y;
    self->parent.height = osd_rectangle_height;
    self->parent.dump = osd_rectangle_dump;

    self->rect = osd_rectangle_rect;
    self->set_rect = osd_rectangle_set_rect;
    TV_TYPE_FP_CHECK(self->rect, self->set_rect);

    return self;
}
