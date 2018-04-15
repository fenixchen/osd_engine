#include "osd_common.h"
#include "osd_ingredient.h"
#include "osd_line.h"
#include "osd_window.h"

struct _osd_line_priv {
    osd_line_hw *line;
};


void osd_line_paint(osd_ingredient *self,
                    osd_window *window,
                    osd_block_hw *block,
                    u32 *window_line_buffer,
                    u32 y) {
    s32 x;
    osd_line_hw *line;
    u32 color;
    s32 x1, x2, y1, y2;
    s32 window_width = window->rect(window).width;
    osd_ingredient *ingredient = (osd_ingredient *)self;
    osd_line *line_self = (osd_line *)self;
    TV_TYPE_GET_PRIV(osd_line_priv, line_self, priv);
    line = priv->line;

    color = ingredient->color(ingredient, line->color);
    x1 = line->x1;
    x2 = line->x2;
    y1 = line->y1;
    y2 = line->y2;

    if (y1 == y2) {
        if (y1 <= (s32)y && (s32)y < y1 + line->weight) {
            for (x = block->x + x1; x < block->x + x2; x ++) {
                if (TV_BETWEEN(x, 0, window_width) &&
                        osd_line_style_check(line->style, x)) {
                    window_line_buffer[x] = color;
                }
            }
        }
    } else if (x1 == x2) {
        for (x = 0; x < line->weight; x ++) {
            if (TV_BETWEEN(x, 0, window_width) &&
                    osd_line_style_check(line->style, y)) {
                window_line_buffer[block->x + x1 + x] = color;
            }
        }
    } else {
        double slope = (double)((int)x2 - (int)x1) / (double)((int)y2 - (int)y1);
        s32 px = x1 + (s32)(slope * ((s32)y - y1));
        if (osd_line_style_check(line->style, y)) {
            for (x = 0; x < line->weight; x++) {
                s32 pos = block->x + px + x;
                if (TV_BETWEEN(pos, 0, window_width)) {
                    window_line_buffer[pos] = color;
                }
            }
        }
    }
}


static u32 osd_line_start_y(osd_ingredient *self) {
    osd_line *line_self = (osd_line *)self;
    osd_line_hw *line = line_self->priv->line;
    return TV_MIN(line->y1, line->y2);
}

static void osd_line_dump(osd_ingredient *ingredient) {
    osd_line_hw *line;
    osd_line *self = (osd_line *)ingredient;
    TV_TYPE_GET_PRIV(osd_line_priv, self, priv);

    line = priv->line;
    TV_LOG("Line\n\tx1:%d, y1:%d, x2:%d, y2:%d\n",
           line->x1, line->y1, line->x2, line->y2);
}

static void osd_line_destroy(osd_ingredient *self) {
    osd_line *line_self = (osd_line *)self;
    FREE_OBJECT(line_self->priv);
    FREE_OBJECT(self);
}

osd_line *osd_line_create(osd_window *window, osd_ingredient_hw *hw) {
    osd_line *self = MALLOC_OBJECT(osd_line);
    self->priv = MALLOC_OBJECT(osd_line_priv);
    self->priv->line = &hw->data.line;

    self->parent.destroy = osd_line_destroy;
    self->parent.paint = osd_line_paint;
    self->parent.destroy = osd_line_destroy;
    self->parent.start_y = osd_line_start_y;
    self->parent.dump = osd_line_dump;
    return self;
}