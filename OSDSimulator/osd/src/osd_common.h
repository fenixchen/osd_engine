#ifndef _OSD_COMMON_H
#define _OSD_COMMON_H

#include "osd_types.h"


unsigned char *osd_read_file(const char *base_folder,
                             const char *filename,
                             u32 *len);

u32 osd_blend_pixel(u32 dst, u32 src, u8 alpha);

int osd_line_style_check(u32 style, u32 x);

u32 osd_color_add(u32 color, u8 r_delta, u8 g_delta, u8 b_delta);

void osd_merge_line(u32 *dst_buf, u32 *src_buf, u16 len, u32 x, u8 alpha);

#endif