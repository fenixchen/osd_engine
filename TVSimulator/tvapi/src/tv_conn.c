#include "tv_conn.h"


struct _osd_conn_priv {
    void *reserved;
};


void tv_conn_destroy(tv_conn *self) {
    TV_ASSERT(self);
    FREE_OBJECT(self);
}

t_bool tv_conn_set_config(tv_conn *self, u32 param, u32 value) {
    return false;
}

t_bool tv_conn_get_config(tv_conn *self, u32 param, u32 *value) {
    return false;
}

t_bool tv_conn_open(tv_conn *self) {
    return false;
}

t_bool tv_conn_w32(tv_conn *self, u32 addr, u32 value) {
    return false;
}

t_bool tv_conn_r32(tv_conn *self, u32 addr, u32 *value) {
    return false;
}

void tv_conn_close(tv_conn *self) {

}

tv_conn *tv_conn_create() {
    tv_conn *self = MALLOC_OBJECT(tv_conn);
    self->destroy = tv_conn_destroy;
    self->set_config = tv_conn_set_config;
    self->get_config = tv_conn_get_config;
    self->open = tv_conn_open;
    self->close = tv_conn_close;
    self->w32 = tv_conn_w32;
    self->r32 = tv_conn_r32;

    TV_TYPE_FP_CHECK(self->destroy, self->close);

    return self;
}