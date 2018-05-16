#include "tfd_api.h"


#define TFD_ATTR_MAX_COUNT 32

struct _tfd_filter {
    tfd_manager *manager;
    u8 attr_count;
    tfd_attr_id attr_array[TFD_ATTR_MAX_COUNT];
    tfd_attr_item_id attr_item_array[TFD_ATTR_MAX_COUNT] ;
};

tfd_filter *tfd_filter_create(tfd_manager *manager) {
    u32 i, checked_item_count;
    const tfd_attr_checked_item * checked_item_array;
    tfd_filter *self = MALLOC_OBJECT(tfd_filter);
    self->manager = manager;

    checked_item_array = tfd_manager_attr_checked_items(manager, &checked_item_count);
    for (i = 0; i < checked_item_count; i ++) {
        const tfd_attr_checked_item *item = &checked_item_array[i];
        if (item->attr_item_count == 1 && item->attr_id) {
            tfd_filter_set(self, item->attr_id, item->attr_item_array[0]);
        }
    }
    return self;
}

void tfd_filter_destroy(tfd_filter *self) {
    FREE_OBJECT(self);
}

void tfd_filter_set(tfd_filter *self, tfd_attr_id attr_id, tfd_attr_item_id attr_item_id) {
    u32 i;
    for (i = 0; i < self->attr_count; i ++) {
        if (self->attr_array[i] == attr_id) {
            self->attr_item_array[i] = attr_item_id;
            return;
        }
    }
    TV_ASSERT(self->attr_count < TFD_ATTR_MAX_COUNT);
    self->attr_array[self->attr_count] = attr_id;
    self->attr_item_array[self->attr_count] = attr_item_id;
    ++ self->attr_count;
}

boolean tfd_filter_get(tfd_filter *self, tfd_attr_id attr_id, tfd_attr_item_id *attr_item_id) {
    u32 i;
    for (i = 0; i < self->attr_count; i ++) {
        if (self->attr_array[i] == attr_id) {
            *attr_item_id = self->attr_item_array[i];
            return true;
        }
    }
    return false;
}

u32 tfd_filter_count(tfd_filter *self) {
    return self->attr_count;
}

void tfd_filter_dump(tfd_filter *self) {
    u32 i;
    for (i = 0; i < self->attr_count; i ++) {
        TV_LOGI("%-16s => %-16s\n",
                tfd_manager_attr_name(self->manager, self->attr_array[i]),
                tfd_manager_attr_item_name(self->manager, self->attr_item_array[i]));
    }
}

void tfd_filter_attr_item(tfd_filter *self, u32 index, tfd_attr_id *attr_id, tfd_attr_item_id *attr_item_id) {
    TV_ASSERT(index < self->attr_count);
    *attr_id = self->attr_array[index];
    *attr_item_id = self->attr_item_array[index];
}