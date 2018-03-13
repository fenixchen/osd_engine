#include "osd_log.h"



const char *ingredient_name[] = {
    "INVALID",
    "RECTANGLE",
    "LINE",
    "CHARACTER",
    "BITMAP",
    "LABEL",
    "FORM",
    "BUTTON",
    "EDIT"
};

void log_scene(osd_scene *scene) {
    OSD_LOG("scene\n\twidth: %d, height:%d, ram_offset:%#x\n",
            scene->hw->width, scene->hw->height, scene->hw->ram_offset);
}

void log_palette(int index, osd_palette *palette) {
    //    u32 i;
    OSD_LOG("palette[%d] \n\tpixel_format:%d, pixel_bits:%d, entry_count:%d, luts_addr:%#x\n",
            index,
            palette->pixel_format, palette->pixel_bits, palette->entry_count,
            (unsigned int)palette->luts_addr);
#if 0
    for (i = 0; i < palette->entry_count; i ++) {
        LOG("\t%08x ", palette->lut[i]);
    }
    LOG("\n");
#endif
}

void log_ingredient(int index, osd_ingredient *ingredient) {
    OSD_LOG("ingredient[%d] \n\ttype:%s(%d), palette:%d\n",
            index, ingredient_name[ingredient->type], ingredient->type, ingredient->palette_index);
    switch (ingredient->type) {
    case OSD_INGREDIENT_RECTANGLE: {
        osd_rectangle *rect = &ingredient->data.rect;
        OSD_LOG("\tgradient_mode:%d, width:%d, height:%d\n"
                "\tborder_color:[%d,%d,%d,%d], border_weight:%d, border_style:%d\n"
                "\tbgcolor:[%d,%d]\n",
                rect->gradient_mode,
                rect->width, rect->height,
                rect->border_color_top, rect->border_color_bottom, rect->border_color_left,
                rect->border_color_right, rect->border_weight, rect->border_style,
                rect->bgcolor_start, rect->bgcolor_end);
        break;
    }
    case OSD_INGREDIENT_LINE: {
        osd_line *line = &ingredient->data.line;
        OSD_LOG("\tx1:%d, y1:%d, x2:%d, y2:%d\n", line->x1, line->y1, line->x2, line->y2);
        break;
    }
    case OSD_INGREDIENT_BITMAP: {
        osd_bitmap *bitmap = &ingredient->data.bitmap;
        OSD_LOG("\twidth:%d, height:%d, count:%d, size:%d, addr:%#x\n",
                bitmap->width, bitmap->height, bitmap->bitmap_count,
                bitmap->data_size, bitmap->data_addr);
        break;
    }
    case OSD_INGREDIENT_CHARACTER: {
        osd_glyph *glyph = (osd_glyph *)ingredient->ram_data;
        osd_character *character = &ingredient->data.character;
        OSD_LOG("\tleft:%d, top:%d, width:%d, height:%d\n"
                "\tchar:%u, color:%d, size:%d, mono:%d, addr:%#x\n",
                glyph->left, glyph->top, glyph->width, glyph->height,
                (unsigned int)glyph->char_code, character->color,
                glyph->data_size, glyph->monochrome, character->glyph_addr);
        break;
    }
    case OSD_INGREDIENT_LABEL:
    case OSD_INGREDIENT_FORM:
    case OSD_INGREDIENT_BUTTON:
    case OSD_INGREDIENT_EDIT:
        break;
    default:
        OSD_ERR("Unknown ingredient:%d\n", ingredient->type);
        TV_ASSERT(0);
    }
}

