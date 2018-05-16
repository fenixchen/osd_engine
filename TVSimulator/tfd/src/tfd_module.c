#include "tfd_api.h"

typedef struct _tfd_attr_prop tfd_attr_prop;
struct _tfd_attr_prop {
    tfd_attr_id attr_id;
    u32 attr_item_count;
    tfd_attr_item_id *attr_item_id_array;
};

typedef struct _tfd_state tfd_state;
struct _tfd_state {
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
};

struct _tfd_module {
    tfd_module *self;
    tfd_module_header header;
    char *name;
    tfd_state *state_array;
    u32 plugin_type;
    u32 data_size;
    u32 attr_prop_count;
    tfd_attr_prop *attr_prop_array;
    union {
        tfd_plugin_regtable *regtable;
    } plugin;
};

void tfd_module_destroy(tfd_module *self) {
    u32 i, j;
    switch (self->plugin_type) {
    case TFD_PLUGIN_REGTABLE:
        tfd_plugin_regtable_destroy(self->plugin.regtable);
        break;
    }
    for (i = 0; i < self->header.state_count; i ++) {
        tfd_state *state = &self->state_array[i];
        for (j = 0; j < state->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &state->attr_prop_array[j];
            FREE_OBJECT(attr_prop->attr_item_id_array);
        }
        FREE_OBJECT(state->attr_prop_array);
    }
    for (j = 0; j < self->attr_prop_count; j ++) {
        tfd_attr_prop *attr_prop = &self->attr_prop_array[j];
        FREE_OBJECT(attr_prop->attr_item_id_array);
    }
    FREE_OBJECT(self->attr_prop_array);
    FREE_OBJECT(self->state_array);
    FREE_OBJECT(self->name);
    FREE_OBJECT(self);
}

void tfd_module_write(tfd_module *self, u32 state_index) {
    TV_ASSERT(state_index < self->header.state_count);
    switch (self->plugin_type) {
    case TFD_PLUGIN_REGTABLE:
        TV_ASSERT(self->plugin.regtable);
        tfd_plugin_regtable_send(self->plugin.regtable, state_index);
        break;
    }
}


static boolean tfd_attr_prop_match(tfd_attr_prop *prop_array, u32 prop_count, tfd_filter *filter) {
    u32 i, j, k;
    tfd_attr_id attr_id;
    tfd_attr_item_id attr_item_id;
    u32 attr_count = tfd_filter_count(filter);
    for (i = 0; i < attr_count; i ++) {
        tfd_filter_attr_item(filter, i, &attr_id, &attr_item_id);
        for (j = 0; j < prop_count; j ++) {
            tfd_attr_prop *prop = &prop_array[j];
            if (prop->attr_id == attr_id) {
                for (k = 0; k < prop->attr_item_count; k ++) {
                    if (prop->attr_item_id_array[k] == attr_item_id) {
                        break;
                    }
                }
                if (k == prop->attr_item_count) {
                    return false;
                }
            }
        }
    }
    return true;
}

void tfd_module_write_filter(tfd_module *self, tfd_filter *filter) {
    u32 i;
    u32 matched_count = 0;
    u32 matched_index = 0;
    u32 attr_count = tfd_filter_count(filter);

    /* check table attribute */
    if (!tfd_attr_prop_match(self->attr_prop_array, self->attr_prop_count, filter)) {
        return;
    }

    /* check state attribute */
    for (i = 0; i < self->header.state_count; i ++) {
        tfd_state *state = &self->state_array[i];
        if (tfd_attr_prop_match(state->attr_prop_array, state->attr_prop_count, filter)) {
            matched_count ++;
            if (matched_count > 1) {
                TV_LOGI("Both state[%d] and state[%d] of %s are matched, ignore the module.\n",
                        matched_index, i, self->name);
                break;
            } else {
                matched_index = i;
            }
        }
    }
    if (matched_count == 1) {
        tfd_module_write(self, matched_index);
    }
}

const char *tfd_module_name(tfd_module *self) {
    return self->name;
}

u32 tfd_module_state_count(tfd_module *self) {
    return self->header.state_count;
}

tfd_module *tfd_module_create(tfd_manager *manager, const u8 **p_offset) {
    u8 name_len;
    u32 i, j, k;
    tfd_module *self = MALLOC_OBJECT(tfd_module);
    const u8 *offset = *p_offset;

    TV_MEMCPY(&self->header, offset, sizeof(self->header));
    offset += sizeof(self->header);

    name_len = *offset;
    offset += 1;
    self->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
    TV_MEMCPY(self->name, offset, name_len);
    offset += name_len;

    /* load tfd state_array */
    self->state_array = MALLOC_OBJECT_ARRAY(tfd_state, self->header.state_count);

    for (i = 0; i < self->header.state_count; i ++) {
        tfd_state *state = &self->state_array[i];

        state->attr_prop_count = *(u32*)offset;
        offset += sizeof(state->attr_prop_count);

        state->attr_prop_array = MALLOC_OBJECT_ARRAY(tfd_attr_prop, state->attr_prop_count);
        for (j = 0; j < state->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &state->attr_prop_array[j];
            attr_prop->attr_id = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            attr_prop->attr_item_count = *(u32*)offset;
            offset += sizeof(u32);
            attr_prop->attr_item_id_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, attr_prop->attr_item_count);
            for (k = 0; k < attr_prop->attr_item_count; k ++) {
                attr_prop->attr_item_id_array[k] = *(tfd_attr_item_id*)offset;
                offset += sizeof(tfd_attr_item_id);
            }
        }
    }
    /* load module data */
    self->plugin_type = *(u32*)offset;
    offset += sizeof(u32);

    self->data_size = *(u32*)offset;
    offset += sizeof(u32);

    switch (self->plugin_type) {
    case TFD_PLUGIN_REGTABLE:
        self->plugin.regtable = tfd_plugin_regtable_create(offset, self->data_size);
        break;
    default:
        TV_LOGE("Unknown plugin:0x%08X\n", self->plugin_type);
        TV_ASSERT(0);
    }
    offset += self->data_size;

    /* load attribute group for module */
    self->attr_prop_count = *(u32*)offset;
    offset += sizeof(self->attr_prop_count);

    if (self->attr_prop_count > 0) {
        self->attr_prop_array = MALLOC_OBJECT_ARRAY(tfd_attr_prop, self->attr_prop_count);
        for (j = 0; j < self->attr_prop_count; j ++) {
            tfd_attr_prop *attr_prop = &self->attr_prop_array[j];
            attr_prop->attr_id = *(tfd_attr_id*)offset;
            offset += sizeof(tfd_attr_id);

            attr_prop->attr_item_count = *(u32*)offset;
            offset += sizeof(u32);
            attr_prop->attr_item_id_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, attr_prop->attr_item_count);
            for (k = 0; k < attr_prop->attr_item_count; k ++) {
                attr_prop->attr_item_id_array[k] = *(tfd_attr_item_id*)offset;
                offset += sizeof(tfd_attr_item_id);
            }
        }
    }

    *p_offset = offset;
    return self;
}
