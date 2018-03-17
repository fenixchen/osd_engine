#include "osd_common.h"

unsigned char *osd_read_file(const char *filename, u32 *len) {
    FILE * fp;
    unsigned int length;
    unsigned char *buffer = NULL;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("open <%s> failed.\n", filename);
        return buffer;
    }
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (unsigned char *)malloc(length);
    if (fread(buffer, length, 1, fp) != 1) {
        printf("read <%s> failed.\n", filename);
        free(buffer);
        return NULL;
    }
    *len = length;
    fclose(fp);

    OSD_LOG("%s, size[%d]\n", filename, length);
    return buffer;
}


u32 osd_blend_pixel(u32 dst, u32 src, u8 alpha) {
    u8 dst_r, dst_g, dst_b;
    u8 src_r, src_g, src_b;
    u8 new_r, new_g, new_b;

    if (alpha == 0xFF) {
        return src;
    }
    if (alpha == 0) {
        return dst;
    }
    dst_r = OSD_R(dst);
    dst_g = OSD_G(dst);
    dst_b = OSD_B(dst);

    src_r = OSD_R(src);
    src_g = OSD_G(src);
    src_b = OSD_B(src);

    new_r = OSD_BLEND(dst_r, src_r, alpha);
    new_g = OSD_BLEND(dst_g, src_g, alpha);
    new_b = OSD_BLEND(dst_b, src_b, alpha);

    return OSD_RGB(new_r, new_g, new_b);
}


int osd_line_style_check(u32 style, u32 x) {
    switch (style) {
    case OSD_LINE_STYLE_SOLID:
        return 1;
    case OSD_LINE_STYLE_DASH:
        return (x % (OSD_LINE_STYLE_DASH_WIDTH + 1)) < OSD_LINE_STYLE_DASH_WIDTH;
    case OSD_LINE_STYLE_DOT1:
        return (x % 2) == 0;
    case OSD_LINE_STYLE_DOT2:
        return (x % 3) == 0;
    case OSD_LINE_STYLE_DOT3:
        return (x % 4) == 0;
    case OSD_LINE_STYLE_DASH_DOT: {
        u32 index = x % (OSD_LINE_STYLE_DASH_WIDTH + 3);
        return index < OSD_LINE_STYLE_DASH_WIDTH || index  == OSD_LINE_STYLE_DASH_WIDTH + 1;
    }
    case OSD_LINE_STYLE_DASH_DOT_DOT: {
        u32 index = x % (OSD_LINE_STYLE_DASH_WIDTH + 5);
        return index < OSD_LINE_STYLE_DASH_WIDTH
               || index  == OSD_LINE_STYLE_DASH_WIDTH + 1
               || index  == OSD_LINE_STYLE_DASH_WIDTH + 3;
    }
    default:
        TV_ASSERT(0);
    }
    return 0;
}

u32 osd_color_add(u32 color, int r_delta, int g_delta, int b_delta) {
    u8 r = OSD_COLOR_CLIP(OSD_R(color) + r_delta);
    u8 g = OSD_COLOR_CLIP(OSD_G(color) + g_delta);
    u8 b = OSD_COLOR_CLIP(OSD_B(color) + b_delta);
    return OSD_RGB(r, g, b);
}

void osd_merge_line(u32 *dst_buf, u32 *src_buf, u16 len, u32 x, u8 alpha) {
    u32 i;
    for (i = x; i < x + len; i ++) {
        if (src_buf[i - x] != 0) {
            dst_buf[i] = osd_blend_pixel(dst_buf[i], src_buf[i - x], alpha);
        }
    }
}

int osd_get_rand_boolean(void) {
    int r;
    static int first = 1;
    if (first) {
        srand((unsigned)time(NULL));
        first = 0;
    }
    r = rand();
    return rand() > (RAND_MAX / 2);
}


void osd_char_to_wchar(wchar *wstr, const char *str) {
    const char *p = str;
    wchar *q = wstr;
    while (*p) {
        *q = *p;
        ++p;
        ++q;
    }
}

int osd_wchar_len(const wchar *wstr) {
    const wchar *p = wstr;
    int len = 0;
    while (p && *p) {
        ++p;
        ++len;
    }
    return len;
}

