#ifndef _OS_LOOPER_H
#define _OS_LOOPER_H

#include "tv_types.h"

typedef struct _os_looper os_looper;

typedef struct _os_looper_priv os_looper_priv;


#define OS_LOOPER_WAIT_INFINITE (u32)-1

struct _os_looper {
    os_looper_priv *priv;
    void (*destroy)(os_looper *self);
    void (*run)(os_looper *self);
    void (*send)(os_looper *self, u32 type, u32 param0, u32 param1);
    void (*post)(os_looper *self, u32 type, u32 param0, u32 param1);
    void (*wait)(os_looper *self, u32 timeout);
};

typedef void (*os_looper_handler)(void *param, u32 type, u32 param0, u32 param1);

os_looper *os_looper_create(os_looper_handler handler, void *param);

#endif