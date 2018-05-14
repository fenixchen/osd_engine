#include <windows.h>
#include "os_mq.h"


struct _os_mq_priv {
    u32 capacity;
    u32 data_size;
    u8 *data;
    u32 read, write;
    int full;
};

static void os_mq_destroy(os_mq *self) {
    TV_TYPE_GET_PRIV(os_mq_priv, self, priv);
    FREE_BUFFER(priv->data);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void os_mq_put_impl(os_mq *self, void *data) {
    TV_TYPE_GET_PRIV(os_mq_priv, self, priv);
    TV_ASSERT(!priv->full);
    TV_MEMCPY(priv->data + priv->write * priv->data_size, data, priv->data_size);

    priv->write = (priv->write + 1) % priv->capacity;
    if (priv->write == priv->read) {
        priv->full = 0;
    }
}

static void* os_mq_get_impl(os_mq *self) {
    u8 *ret;
    TV_TYPE_GET_PRIV(os_mq_priv, self, priv);
    TV_ASSERT(priv->read != priv->write || priv->full);
    ret = priv->data + priv->read * priv->data_size;
    priv->read = (priv->read + 1) % priv->capacity;
    return ret;
}

static int os_mq_full_impl(os_mq *self) {
    TV_TYPE_GET_PRIV(os_mq_priv, self, priv);
    return priv->full;
}

static int os_mq_empty_impl(os_mq *self) {
    TV_TYPE_GET_PRIV(os_mq_priv, self, priv);
    return priv->read == priv->write && !priv->full;
}

os_mq *os_mq_create(u32 capacity, u32 data_size) {
    os_mq_priv *priv;
    os_mq *self = MALLOC_OBJECT(os_mq);
    priv = MALLOC_OBJECT(os_mq_priv);
    self->priv = priv;
    priv->capacity = capacity;
    priv->data_size = data_size;
    self->destroy = os_mq_destroy;
    self->put = os_mq_put_impl;
    self->get = os_mq_get_impl;
    self->full = os_mq_full_impl;
    self->empty = os_mq_empty_impl;
    TV_TYPE_FP_CHECK(self->destroy, self->empty);

    priv->data = MALLOC_BUFFER(priv->capacity * priv->data_size);

    return self;
}




