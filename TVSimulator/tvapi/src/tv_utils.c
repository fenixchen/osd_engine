#include "tv_conn.h"

u8 *tv_read_file(const char *filename, u32 *len) {
    FILE * fp;
    unsigned int length;
    unsigned char *buffer = NULL;
    fp = fopen(filename, "rb");
    if (fp == NULL) {
        TV_ERR("open <%s> failed.\n", filename);
        return buffer;
    }
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (unsigned char *)malloc(length);
    if (fread(buffer, length, 1, fp) != 1) {
        TV_ERR("read <%s> failed.\n", filename);
        free(buffer);
        return NULL;
    }
    *len = length;
    fclose(fp);

    TV_LOG("%s, size[%d]\n", filename, length);
    return buffer;
}

int tv_get_rand_boolean(void) {
    int r;
    static int first = 1;
    if (first) {
        srand((unsigned)time(NULL));
        first = 0;
    }
    r = rand();
    return rand() > (RAND_MAX / 2);
}


void tv_char_to_wchar(t_wchar *wstr, const char *str) {
    const char *p = str;
    t_wchar *q = wstr;
    while (*p) {
        *q = *p;
        ++p;
        ++q;
    }
    *q = '\0';
}

int tv_wchar_len(const t_wchar *wstr) {
    const t_wchar *p = wstr;
    int len = 0;
    while (p && *p) {
        ++p;
        ++len;
    }
    return len;
}

