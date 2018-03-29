#ifndef _TV_UTILS_H
#define _TV_UTILS_H

#include "tv_types.h"

EXTERNC u8 *tv_read_file(const char *filename, u32 *len);

EXTERNC int tv_get_rand_boolean(void);

EXTERNC void tv_char_to_wchar(t_wchar *wstr, const char *str);

EXTERNC int tv_wchar_len(const t_wchar *wstr);

#endif

