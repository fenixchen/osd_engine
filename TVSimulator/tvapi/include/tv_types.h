#ifndef _TV_TYPES_H
#define _TV_TYPES_H

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifdef WIN32
#include <vld.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <io.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef unsigned short t_wchar;

typedef int t_bool;

#define false 0

#define true !false

#define MALLOC_OBJECT(type) (type*)calloc(sizeof(type), 1)
#define MALLOC_OBJECT_ARRAY(type, count) (type*)calloc(sizeof(type), count)
#define FREE_OBJECT(obj) do { free(obj); obj = NULL; } while (0)
#define MALLOC_BUFFER(size) calloc(size, 1)
#define FREE_BUFFER(p) do { free(p); p = NULL; } while (0)


#define TV_TYPE_GET_PRIV(type, self, var)	\
	type *var;								\
	TV_ASSERT(self);						\
	var = self->priv;						\
	TV_ASSERT(var);							\


#define TV_TYPE_FP_CHECK(fp_start, fp_end) {\
    u32 *fp;\
    for (fp = (u32*)&fp_start; fp <= (u32*)&fp_end; ++ fp) {\
        TV_ASSERT(*fp);\
    }\
}

#define TV_TYPE_DESTROY(p) do { if (p) { p->destroy(p); p = NULL; } } while (0)

#ifndef TV_MAX_PATH
#define TV_MAX_PATH 256
#endif


#define TV_LOG printf

#define TV_ERR printf

#define TV_ASSERT assert

#define TV_OFFSET_OF(type, member)   (size_t)&(((type *)0)->member)


#define TV_MAX(x, y) ((x) > (y) ? (x) : (y))
#define TV_MIN(x, y) ((x) < (y) ? (x) : (y))

#define TV_CLIP(x, lval, uval) TV_MIN(TV_MAX(x, lval), uval)

#define TV_BETWEEN(x, lval, uval) ((x) >= (lval) && (x) < (uval))

#define TV_MEMCPY memcpy


#endif

