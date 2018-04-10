#ifndef _OS_THREAD_H
#define _OS_THREAD_H

#include "tv_types.h"

typedef struct _os_thread os_thread;

typedef struct _os_thread_priv os_thread_priv;


#define OS_THREAD_WAIT_INFINITE (u32)-1

struct _os_thread {
    os_thread_priv *priv;
    void (*destroy)(os_thread *self);
    void (*run)(os_thread *self);
    void (*wait)(os_thread *self, u32 timeout);
};

typedef void * (*os_thread_func)(void *param);

os_thread *os_thread_create(os_thread_func thread_func, void *param);

#define os_thread_wait(thread) thread->wait(thread, OS_THREAD_WAIT_INFINITE)

#define os_thread_run(thread) thread->run(thread)

#endif