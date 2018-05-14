#include "tfd_manager.h"


static tfd_group *tfd_group_create(const u8 **p_offset);
static tfd_module *tfd_module_create(const u8 **p_offset);

typedef struct _tfd_attr_group tfd_attr_group;
struct _tfd_attr_group {
    tfd_attr_id attr_id;
    tfd_attr_item_id attr_group_id;
    u32 attr_item_id_count;
    tfd_attr_item_id *attr_item_id_array;
};

typedef struct _tfd_attr_relation tfd_attr_relation;
struct _tfd_attr_relation {
    tfd_attr_id attr_id1, attr_id2;
    u32 attr_pair_count;
    tfd_attr_item_id *attr_item1_array;
    tfd_attr_item_id *attr_item2_array;
};

typedef struct _tfd_attr_id_name tfd_attr_id_name;
struct _tfd_attr_id_name {
    tfd_attr_id attr_id;
    char *name;
};

typedef struct _tfd_attr_item_id_name tfd_attr_item_id_name;
struct _tfd_attr_item_id_name {
    tfd_attr_item_id attr_item_id;
    char *name;
};


struct _tfd_manager_priv {
    tfd_header header;
    u32 group_count;
    tfd_group **group_array;

    u32 attr_group_count;
    tfd_attr_group *attr_group_array;

    u32 attr_relation_count;
    tfd_attr_relation *attr_relation_array;

    u32 attr_id_name_count;
    tfd_attr_id_name *attr_id_name_array;

    u32 attr_item_id_name_count;
    tfd_attr_item_id_name *attr_item_id_name_array;
};

struct _tfd_group_priv {
    tfd_group_header header;
    char *name;
    tfd_module **modules;
};

typedef struct _tfd_attr_prop tfd_attr_prop;
struct _tfd_attr_prop {
    tfd_attr_id attr_id;
    u32 attr_count;
    tfd_attr_item_id *attr_item_id_array;
};

typedef struct _tfd_state tfd_state;
struct _tfd_state {
    tfd_state_header header;
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
};

struct _tfd_module_priv {
    tfd_module *self;
    tfd_module_header header;
    char *name;
    u32 state_count;
    tfd_state *state_array;
    u32 fw_plugin_id;
    u32 data_size;
    u8 *data;
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
};

static void tfd_manager_destroy(tfd_manager *self) {
    u32 i;
    tfd_manager_priv *priv = self->priv;
    for (i = 0; i < priv->header.group_count; i ++) {
        priv->group_array[i]->destroy(priv->group_array[i]);
    }

    for (i = 0; i < priv->attr_group_count; i ++) {
        FREE_OBJECT(priv->attr_group_array[i].attr_item_id_array);
    }
    FREE_OBJECT(priv->attr_group_array);

    for (i = 0; i < priv->attr_relation_count; i ++) {
        FREE_OBJECT(priv->attr_relation_array[i].attr_item1_array);
        FREE_OBJECT(priv->attr_relation_array[i].attr_item2_array);
    }
    FREE_OBJECT(priv->attr_relation_array);

    for (i = 0; i < priv->attr_id_name_count; i ++) {
        FREE_OBJECT(priv->attr_id_name_array[i].name);
    }
    FREE_OBJECT(priv->attr_id_name_array);

    for (i = 0; i < priv->attr_item_id_name_count; i ++) {
        FREE_OBJECT(priv->attr_item_id_name_array[i].name);
    }
    FREE_OBJECT(priv->attr_item_id_name_array);

    FREE_OBJECT(priv->group_array);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void tfd_manager_load(tfd_manager *self, u8 *data, u32 size) {
    u32 i, j;
    u8 *offset = data;
    TV_TYPE_GET_PRIV(tfd_manager_priv, self, priv);
    TV_MEMCPY(&priv->header, offset, sizeof(priv->header));
    offset += sizeof(priv->header);

    priv->group_count = *(u32*)(offset);
    offset += sizeof(u32);

    priv->group_array = MALLOC_OBJECT_ARRAY(tfd_group*, priv->group_count);

    for (i = 0; i < priv->group_count; i ++) {
        priv->group_array[i] = tfd_group_create(&offset);
    }

    offset += sizeof(u16) +  sizeof(u16); //ignore header

    priv->attr_group_count = *(u32*)offset;
    offset += sizeof(u32);

    if (priv->attr_group_count > 0) {
        priv->attr_group_array = MALLOC_OBJECT_ARRAY(tfd_attr_group, priv->attr_group_count);
        for (i = 0; i < priv->attr_group_count; i ++) {
            tfd_attr_group *attr_group = &priv->attr_group_array[i];
            attr_group->attr_id = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            attr_group->attr_group_id = *(tfd_attr_item_id*)offset;
            offset += sizeof(tfd_attr_item_id);

            attr_group->attr_item_id_count = *(u32*)offset;
            offset += sizeof(u32);

            attr_group->attr_item_id_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, attr_group->attr_item_id_count);
            for (j = 0; j < attr_group->attr_item_id_count; j ++) {
                attr_group->attr_item_id_array[j] = *(tfd_attr_item_id*)offset;
                offset += 4;
            }
        }
    }

    priv->attr_relation_count = *(u32*)offset;
    offset += sizeof(u32);
    if (priv->attr_relation_count > 0) {
        priv->attr_relation_array = MALLOC_OBJECT_ARRAY(tfd_attr_relation, priv->attr_relation_count);
        for (i = 0; i < priv->attr_relation_count; i ++) {
            tfd_attr_relation *relation = &priv->attr_relation_array[i];

            relation->attr_id1 = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            relation->attr_id2 = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            relation->attr_pair_count = *(u32*)offset;
            offset += sizeof(u32);

            relation->attr_item1_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, relation->attr_pair_count);
            relation->attr_item2_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, relation->attr_pair_count);

            for (j = 0; j < relation->attr_pair_count; j ++) {
                relation->attr_item1_array[j] = *(tfd_attr_item_id*)offset;
                offset += 4;
                relation->attr_item2_array[j] = *(tfd_attr_item_id*)offset;
                offset += 4;
            }
        }
    }

    priv->attr_id_name_count = *(u32*)offset;
    offset += sizeof(u32);
    priv->attr_id_name_array = MALLOC_OBJECT_ARRAY(tfd_attr_id_name, priv->attr_id_name_count);

    for (i = 0; i < priv->attr_id_name_count; i ++) {
        u32 name_len;
        tfd_attr_id_name *attr_id_name = &priv->attr_id_name_array[i];
        attr_id_name->attr_id = *(tfd_attr_id*)offset;
        offset += sizeof(tfd_attr_id);

        name_len = *(u8*)offset;
        offset += sizeof(u8);

        attr_id_name->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
        TV_MEMCPY(attr_id_name->name, offset, name_len);
        offset += name_len;
    }

    priv->attr_item_id_name_count = *(u32*)offset;
    offset += sizeof(u32);
    priv->attr_item_id_name_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id_name, priv->attr_item_id_name_count);

    for (i = 0; i < priv->attr_item_id_name_count; i ++) {
        u32 name_len;
        tfd_attr_item_id_name *attr_item_id_name = &priv->attr_item_id_name_array[i];
        attr_item_id_name->attr_item_id = *(tfd_attr_item_id*)offset;
        offset += sizeof(tfd_attr_item_id);

        name_len = *(u8*)offset;
        offset += sizeof(u32);

        attr_item_id_name->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
        TV_MEMCPY(attr_item_id_name->name, offset, name_len);
        offset += name_len;
    }
}
tfd_manager *tfd_manager_create(u8 *data, u32 size) {
    tfd_manager *self = MALLOC_OBJECT(tfd_manager);
    self->priv = MALLOC_OBJECT(tfd_manager_priv);
    self->destroy = tfd_manager_destroy;
    TV_TYPE_FP_CHECK(self->destroy, self->destroy);
    tfd_manager_load(self, data, size);
    return self;
}

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

static tfd_group *tfd_group_create(const u8 **p_offset) {
    u32 module_count, i;
    u8 name_len;
    tfd_group_priv *priv;
    const u8 *offset = *p_offset;
    tfd_group *self = MALLOC_OBJECT(tfd_group);
    self->priv = MALLOC_OBJECT(tfd_group_priv);
    priv = self->priv;
    self->destroy = tfd_group_destroy;
    TV_TYPE_FP_CHECK(self->destroy, self->destroy);

    TV_MEMCPY(&priv->header, offset, sizeof(priv->header));
    offset += sizeof(priv->header);

    name_len = *offset;
    offset += 1;
    priv->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
    TV_MEMCPY(priv->name, offset, name_len);

    priv->name[name_len] = '\0';
    offset += name_len;

    module_count = *(u32*)offset;
    offset += sizeof(module_count);

    priv->modules = MALLOC_OBJECT_ARRAY(tfd_module*, module_count);
    for (i = 0; i < module_count; i ++) {
        priv->modules[i] = tfd_module_create(&offset);
    }


    *p_offset = offset;
    return self;
}

static void tfd_module_destroy(tfd_module *self) {
    u32 i, j;
    FREE_OBJECT(self->priv->data);
    for (i = 0; i < self->priv->state_count; i ++) {
        tfd_state *state = &self->priv->state_array[i];
        for (j = 0; j < state->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &state->attr_prop_array[j];
            FREE_OBJECT(attr_prop->attr_item_id_array);
        }
        FREE_OBJECT(state->attr_prop_array);
    }
    for (j = 0; j < self->priv->attr_prop_count; j ++) {
        tfd_attr_prop *attr_prop = &self->priv->attr_prop_array[j];
        FREE_OBJECT(attr_prop->attr_item_id_array);
    }
    FREE_OBJECT(self->priv->attr_prop_array);
    FREE_OBJECT(self->priv->state_array);
    FREE_OBJECT(self->priv->name);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}

static void tfd_module_send(tfd_module *self, u8 index) {

}

static tfd_module *tfd_module_create(const u8 **p_offset) {
    u8 name_len;
    u32 i, j, k;
    tfd_module *self = MALLOC_OBJECT(tfd_module);
    tfd_module_priv *priv = MALLOC_OBJECT(tfd_module_priv);
    const u8 *offset = *p_offset;
    self->priv = priv;
    priv->self = self;
    self->destroy = tfd_module_destroy;
    self->send = tfd_module_send;
    TV_TYPE_FP_CHECK(self->destroy, self->send);

    TV_MEMCPY(&priv->header, offset, sizeof(priv->header));
    offset += sizeof(priv->header);

    name_len = *offset;
    offset += 1;
    priv->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
    TV_MEMCPY(priv->name, offset, name_len);
    offset += name_len;

    /* load tfd state_array */
    priv->state_count = *(u32*)offset;
    offset += sizeof(priv->state_count);

    priv->state_array = MALLOC_OBJECT_ARRAY(tfd_state, priv->state_count);

    for (i = 0; i < priv->state_count; i ++) {
        tfd_state *state = &priv->state_array[i];
        TV_MEMCPY(&state->header, offset, sizeof(state->header));
        offset += sizeof(state->header);

        offset += sizeof(u16) + sizeof(u16); //ignore attr_prop header

        state->attr_prop_count = *(u32*)offset;
        offset += sizeof(state->attr_prop_count);

        state->attr_prop_array = MALLOC_OBJECT_ARRAY(tfd_attr_prop, state->attr_prop_count);
        for (j = 0; j < state->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &state->attr_prop_array[j];
            attr_prop->attr_id = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            attr_prop->attr_count = *(u32*)offset;
            offset += sizeof(u32);
            attr_prop->attr_item_id_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, attr_prop->attr_count);
            for (k = 0; k < attr_prop->attr_count; k ++) {
                attr_prop->attr_item_id_array[k] = *(tfd_attr_item_id*)offset;
                offset += sizeof(tfd_attr_item_id);
            }
        }
    }
    /* load module data */
    priv->fw_plugin_id = *(u32*)offset;
    offset += sizeof(u32);

    TV_ASSERT(*(u32*)offset == 1);
    offset += sizeof(u32);

    offset += sizeof(u32); //ignore chip_id

    priv->data_size = *(u32*)offset;
    offset += sizeof(u32);

    priv->data = MALLOC_OBJECT_ARRAY(u8, priv->data_size);
    TV_MEMCPY(priv->data, offset, priv->data_size);
    offset += priv->data_size;

    /* load attribute group for module */
    offset += sizeof(u16) + sizeof(u16); //ignore attr_prop header

    priv->attr_prop_count = *(u32*)offset;
    offset += sizeof(priv->attr_prop_count);

    if (priv->attr_prop_count > 0) {
        priv->attr_prop_array = MALLOC_OBJECT_ARRAY(tfd_attr_prop, priv->attr_prop_count);
        for (j = 0; j < priv->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &priv->attr_prop_array[j];
            attr_prop->attr_id = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            attr_prop->attr_count = *(u32*)offset;
            offset += sizeof(u32);
            attr_prop->attr_item_id_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, attr_prop->attr_count);
            for (k = 0; k < attr_prop->attr_count; k ++) {
                attr_prop->attr_item_id_array[k] = *(tfd_attr_item_id*)offset;
                offset += sizeof(tfd_attr_item_id);
            }
        }
    }

    *p_offset = offset;
    return self;
}
