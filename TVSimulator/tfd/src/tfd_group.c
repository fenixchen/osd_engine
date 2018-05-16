#include "tfd_api.h"


struct _tfd_group {
    tfd_group_header header;
    char *name;
    tfd_module **modules;
};

void tfd_group_destroy(tfd_group *self) {
    u32 i;
    for (i = 0; i < self->header.module_count; i++) {
        tfd_module_destroy(self->modules[i]);
    }
    FREE_OBJECT(self->modules);
    FREE_OBJECT(self->name);
    FREE_OBJECT(self);
}


const char * tfd_group_name(tfd_group *self) {
    return self->name;
}
u32 tfd_group_module_count(tfd_group *self) {
    return self->header.module_count;
}
tfd_module *tfd_group_module(tfd_group *self, u32 module_index) {
    TV_ASSERT(module_index < self->header.module_count);
    return self->modules[module_index];
}

tfd_group *tfd_group_create(tfd_manager *manager, const u8 **p_offset) {
    u32 i;
    u8 name_len;
    const u8 *offset = *p_offset;
    tfd_group *self = MALLOC_OBJECT(tfd_group);

    TV_MEMCPY(&self->header, offset, sizeof(self->header));
    offset += sizeof(self->header);

    name_len = *offset;
    offset += 1;
    self->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
    TV_MEMCPY(self->name, offset, name_len);

    self->name[name_len] = '\0';
    offset += name_len;

    self->modules = MALLOC_OBJECT_ARRAY(tfd_module*, self->header.module_count);
    for (i = 0; i < self->header.module_count; i ++) {
        self->modules[i] = tfd_module_create(manager, &offset);
    }


    *p_offset = offset;
    return self;
}
