#include "osd_ingredient.h"
#include "osd_rectangle.h"
#include "osd_line.h"
#include "osd_bitmap.h"
#include "osd_glyph.h"

u32 osd_ingredient_get_color(osd_scene *scene, osd_window *window,
                             osd_ingredient *ingredient,
                             u32 index) {
    osd_palette *palette;
    u8 palette_index = ingredient->palette_index;
    if (palette_index == OSD_PALETTE_INDEX_INVALID) {
        palette_index = window->palette_index;
    }
    assert(palette_index != OSD_PALETTE_INDEX_INVALID);
    palette = scene->palettes[palette_index];
    assert(palette);
    assert(index < palette->entry_count);
    return palette->lut[index];
}


u32 osd_ingredient_get_color2(osd_scene *scene, osd_window *window,
                              osd_ingredient *ingredient,
                              u8 *color_ram,
                              u32 index) {
    u32 color_index;
    osd_palette *palette;
    u8 palette_index = ingredient->palette_index;
    if (palette_index == OSD_PALETTE_INDEX_INVALID) {
        palette_index = window->palette_index;
    }
    assert(palette_index != OSD_PALETTE_INDEX_INVALID);
    palette = scene->palettes[palette_index];
    assert(palette);
    if (palette->pixel_bits == 8) {
        color_index = color_ram[index * 2];
    } else if (palette->pixel_bits == 16) {
        color_index = (color_ram[index * 2 + 1] << 8) | color_ram[index * 2];
    } else {
        assert(0);
    }
    assert(color_index < palette->entry_count);
    return palette->lut[color_index];
}


u32 osd_ingredient_start_y(osd_ingredient *ingredient) {
    assert(ingredient);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE:
    case OSD_INGREDIENT_BITMAP:
        return 0;
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        return OSD_MIN(line->y1, line->y2);
    }
    case OSD_INGREDIENT_GLYPH: {
        osd_glyph *glyph = &ingredient->data.glyph;
        return glyph->top;
    }
    default:
        assert(0);
    }
    return 0;
}

u32 osd_ingredient_height(osd_ingredient *ingredient, osd_window *window) {
    assert(ingredient);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE: {
        osd_rectangle *rect = &ingredient->data.rect;
        if (rect->height == 0xFFFF)
            return window->height;
        else
            return rect->height;
    }
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        if (line->y2 == line->y1)
            return line->y2 - line->y1 + 1 + line->weight;
        else if (line->y2 > line->y1)
            return line->y2 - line->y1 + 1;
        else
            return line->y1 - line->y2 + 1;
    }
    case OSD_INGREDIENT_BITMAP: {
        osd_bitmap *bitmap = &ingredient->data.bitmap;
        return bitmap->height;
    }
    case OSD_INGREDIENT_GLYPH: {
        osd_glyph *glyph = &ingredient->data.glyph;
        return glyph->height;
    }
    default:
        assert(0);
    }
    return 0;
}


void osd_ingredient_paint(osd_scene *scene,
                          osd_window *window,
                          osd_block *block,
                          osd_ingredient *ingredient,
                          u32 *window_line_buffer,
                          u32 y) {
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE:
        osd_rectangle_paint(scene, window, block, ingredient,
                            window_line_buffer,
                            y);
        break;
    case OSD_INGREDIENT_LINE:
        osd_line_paint(scene, window, block, ingredient,
                       window_line_buffer,
                       y);
        break;
    case OSD_INGREDIENT_BITMAP:
        osd_bitmap_paint(scene, window, block, ingredient,
                         window_line_buffer,
                         y);
        break;
    case OSD_INGREDIENT_GLYPH:
        osd_glyph_paint(scene, window, block, ingredient,
                        window_line_buffer,
                        y);
        break;
    default:
        assert(0);
    }
}