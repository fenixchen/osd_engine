#include "tv_types.h"
#include "tv_log.h"


/* Supported colors. */
#define CLR_NORMAL  "\x1B[0m"
#define CLR_RED     "\x1B[31m"
#define CLR_GREEN   "\x1B[32m"
#define CLR_YELLOW  "\x1B[33m"
#define CLR_BLUE    "\x1B[34m"
#define CLR_NAGENTA "\x1B[35m"
#define CLR_CYAN    "\x1B[36m"
#define CLR_WHITE   "\x1B[37m"
#define CLR_RESET   "\033[0m"

#ifndef TV_LOG_BUF_SZ
#define TV_LOG_BUF_SZ 512
#endif



int _tv_log_global_output_lvl = TV_LOG_VERBOSE;

static const char *log_str[] = {
    "V",
    "I",
    "W",
    "E",
    "F",
};

static const char *log_color[] = {
    CLR_NORMAL,
    CLR_GREEN,
    CLR_YELLOW,
    CLR_RED,
    CLR_NAGENTA,
};

#define TV_LOG_COLOR 1

static void tv_log_write_impl(int level, const char *func,
                              const char *filename, int line,
                              const char *fmt, va_list va) {
    int psize;
    char log_buf[TV_LOG_BUF_SZ];
    char *p = log_buf;
#if TV_LOG_COLOR
    psize = sprintf(p, log_color[level]);
    p += psize;
#endif
    psize = sprintf(p, "[%s]%-16s:%-4d ", log_str[level], filename, line);
    p += psize;

    psize = vsprintf(p, fmt, va);
    p += psize;

#if TV_LOG_COLOR
    psize = sprintf(p, CLR_RESET);
    p += psize;
#endif

    printf(log_buf);
}


void tv_log_write(int level, const char *func, const char *filename, int line, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    tv_log_write_impl(level, func, filename, line, fmt, va);
    va_end(va);
}