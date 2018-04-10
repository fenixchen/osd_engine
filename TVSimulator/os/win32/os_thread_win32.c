#include <windows.h>
#include "os_thread.h"


struct _os_thread_priv {
    os_thread_func thread_func;
    void *thread_param;
    HANDLE thread_handle;
    u32 thread_id;
};


static void os_thread_destroy(os_thread *self) {
    TV_TYPE_GET_PRIV(os_thread_priv, self, priv);
    if (priv->thread_handle) {
        self->wait(self, OS_THREAD_WAIT_INFINITE);
    }
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void os_thread_run_impl(os_thread *self) {
    TV_TYPE_GET_PRIV(os_thread_priv, self, priv);
    priv->thread_handle = CreateThread(NULL,
                                       0,
                                       (LPTHREAD_START_ROUTINE)priv->thread_func,
                                       priv->thread_param,
                                       0,
                                       (LPDWORD)&priv->thread_id);
    TV_ASSERT(priv->thread_handle);
}

static void os_thread_wait_impl(os_thread *self, u32 timeout) {
    TV_TYPE_GET_PRIV(os_thread_priv, self, priv);
    TV_ASSERT(priv->thread_handle);
    WaitForSingleObject(priv->thread_handle, timeout);
    priv->thread_handle = NULL;
}

os_thread *os_thread_create(os_thread_func thread_func, void *param) {
    os_thread_priv *priv;
    os_thread *self = MALLOC_OBJECT(os_thread);
    priv = MALLOC_OBJECT(os_thread_priv);
    self->priv = priv;
    priv->thread_func = thread_func;
    priv->thread_param = param;
    self->destroy = os_thread_destroy;
    self->run = os_thread_run_impl;
    self->wait = os_thread_wait_impl;
    TV_TYPE_FP_CHECK(self->destroy, self->wait);
    return self;
}




