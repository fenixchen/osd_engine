#include "tfd_internal.h"


struct _tfd_group_priv {
    tfd_group_header header;
    char *name;
    tfd_module **modules;
};

static void tfd_group_destroy(tfd_group *self) {
    u32 i;
    for (i = 0; i < self->priv->header.module_count; i++) {
        self->priv->modules[i]->destroy(self->priv->modules[i]);
    }
    FREE_OBJECT(self->priv->modules);
    FREE_OBJECT(self->priv->name);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}


static const char * tfd_group_name(tfd_group *self) {
    return NULL;
}
static u32 tfd_group_module_count(tfd_group *self) {
    return 0;
}
static tfd_module *tfd_group_module(tfd_group *self, u32 module_index) {
    return NULL;
}

tfd_group *tfd_group_create(tfd_manager *manager, const u8 **p_offset) {
    u32 i;
    u8 name_len;
    tfd_group_priv *priv;
    const u8 *offset = *p_offset;
    tfd_group *self = MALLOC_OBJECT(tfd_group);
    self->priv = MALLOC_OBJECT(tfd_group_priv);
    priv = self->priv;
    self->destroy = tfd_group_destroy;
    self->name = tfd_group_name;
    self->module_count = tfd_group_module_count;
    self->module = tfd_group_module;
    TV_TYPE_FP_CHECK(self->destroy, self->module);

    TV_MEMCPY(&priv->header, offset, sizeof(priv->header));
    offset += sizeof(priv->header);

    name_len = *offset;
    offset += 1;
    priv->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
    TV_MEMCPY(priv->name, offset, name_len);

    priv->name[name_len] = '\0';
    offset += name_len;

    priv->modules = MALLOC_OBJECT_ARRAY(tfd_module*, priv->header.module_count);
    for (i = 0; i < priv->header.module_count; i ++) {
        priv->modules[i] = tfd_module_create(manager, &offset);
    }


    *p_offset = offset;
    return self;
}
