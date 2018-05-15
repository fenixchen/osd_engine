#include "tfd_internal.h"

typedef struct _tfd_attr_prop tfd_attr_prop;
struct _tfd_attr_prop {
    tfd_attr_id attr_id;
    u32 attr_count;
    tfd_attr_item_id *attr_item_id_array;
};

typedef struct _tfd_state tfd_state;
struct _tfd_state {
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
};

struct _tfd_module_priv {
    tfd_module *self;
    tfd_module_header header;
    char *name;
    tfd_state *state_array;
    u32 fw_plugin_id;
    u32 data_size;
    u8 *data;
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
};

static void tfd_module_destroy(tfd_module *self) {
    u32 i, j;
    FREE_OBJECT(self->priv->data);
    for (i = 0; i < self->priv->header.state_count; i ++) {
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

static void tfd_module_send(tfd_module *self, u32 state_index) {

}

static const char *tfd_module_name(tfd_module *self) {
    return 0;
}

static u32 tfd_module_state_count(tfd_module *self) {
    return 0;
}

tfd_module *tfd_module_create(tfd_manager *manager, const u8 **p_offset) {
    u8 name_len;
    u32 i, j, k;
    tfd_module *self = MALLOC_OBJECT(tfd_module);
    tfd_module_priv *priv = MALLOC_OBJECT(tfd_module_priv);
    const u8 *offset = *p_offset;
    self->priv = priv;
    priv->self = self;
    self->destroy = tfd_module_destroy;
    self->name = tfd_module_name;
    self->state_count = tfd_module_state_count;
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
    priv->state_array = MALLOC_OBJECT_ARRAY(tfd_state, priv->header.state_count);

    for (i = 0; i < priv->header.state_count; i ++) {
        tfd_state *state = &priv->state_array[i];

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

    priv->data_size = *(u32*)offset;
    offset += sizeof(u32);

    priv->data = MALLOC_OBJECT_ARRAY(u8, priv->data_size);
    TV_MEMCPY(priv->data, offset, priv->data_size);
    offset += priv->data_size;

    /* load attribute group for module */
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
