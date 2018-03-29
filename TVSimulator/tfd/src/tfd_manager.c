#include "tfd_manager.h"

struct _tfd_manager_priv {
    void *reserved;
};

static void tfd_manager_destroy(tfd_manager *self) {
    TV_ASSERT(self);
    TV_LOG("tfd_manager_create:%p\n", self);
    FREE_OBJECT(self);
}

static t_bool tfd_manager_load(tfd_manager *self, u8 *data, u32 size) {
    return false;
}

tfd_manager *tfd_manager_create() {
    tfd_manager *self = MALLOC_OBJECT(tfd_manager);
    self->destroy = tfd_manager_destroy;
    self->load = tfd_manager_load;

    TV_TYPE_FP_CHECK(self->destroy, self->load);

    TV_LOG("tfd_manager_create:%p\n", self);
    return self;
}