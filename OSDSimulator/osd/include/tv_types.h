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


#define TV_ASSERT(p) assert(p)

#define MALLOC_OBJECT(type) (type*)calloc(sizeof(type), 1)
#define MALLOC_OBJECT_ARRAY(type, count) (type*)calloc(sizeof(type), count)
#define FREE_OBJECT(obj) do { free(obj); obj = NULL; } while (0)


#define TV_TYPE_GET_PRIV(type, self, var)	\
	type *var;								\
	TV_ASSERT(self);						\
	var = self->priv;						\
	TV_ASSERT(var);							\

#endif

