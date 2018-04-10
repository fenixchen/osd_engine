#include <windows.h>
#include "os_looper.h"

struct _os_looper_priv {
    os_looper_handler handler;
    void *thread_param;
    HANDLE thread_handle;
    u32 thread_id;
};

static void os_looper_destroy(os_looper *self) {
    TV_TYPE_GET_PRIV(os_looper_priv, self, priv);
    if (priv->thread_handle) {
        self->wait(self, OS_LOOPER_WAIT_INFINITE);
    }
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void os_looper_run_impl(os_looper *self) {
    TV_TYPE_GET_PRIV(os_looper_priv, self, priv);
    priv->thread_handle = CreateThread(NULL,
                                       0,
                                       (LPTHREAD_START_ROUTINE)priv->handler,
                                       priv->thread_param,
                                       0,
                                       (LPDWORD)&priv->thread_id);
    TV_ASSERT(priv->thread_handle);
}

static void os_looper_wait_impl(os_looper *self, u32 timeout) {
    TV_TYPE_GET_PRIV(os_looper_priv, self, priv);
    TV_ASSERT(priv->thread_handle);
    WaitForSingleObject(priv->thread_handle, timeout);
    priv->thread_handle = NULL;
}


static void os_looper_send_impl(os_looper *self, u32 type, u32 param0, u32 param1) {

}

static void os_looper_post_impl(os_looper *self, u32 type, u32 param0, u32 param1) {

}

os_looper *os_looper_create(os_looper_handler handler, void *param) {
    os_looper_priv *priv;
    os_looper *self = MALLOC_OBJECT(os_looper);
    priv = MALLOC_OBJECT(os_looper_priv);
    self->priv = priv;
    priv->handler = handler;
    priv->thread_param = param;
    self->destroy = os_looper_destroy;
    self->send = os_looper_send_impl;
    self->post = os_looper_post_impl;
    self->run = os_looper_run_impl;
    self->wait = os_looper_wait_impl;
    TV_TYPE_FP_CHECK(self->destroy, self->wait);
    return self;
}


