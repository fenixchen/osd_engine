#include "tfd_api.h"

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

struct _tfd_manager {
    tfd_header header;
    tfd_group **group_array;

    u32 attr_group_count;
    tfd_attr_group *attr_group_array;

    u32 attr_relation_count;
    tfd_attr_relation *attr_relation_array;

    u32 attr_id_name_count;
    tfd_attr_id_name *attr_id_name_array;

    u32 attr_item_id_name_count;
    tfd_attr_item_id_name *attr_item_id_name_array;

    u32 attr_checked_item_count;
    tfd_attr_checked_item *attr_checked_item_array;

    u32 table_count;
    tfd_table **table_array;
};

void tfd_manager_destroy(tfd_manager *self) {
    u32 i;
    for (i = 0; i < self->header.group_count; i ++) {
        tfd_group_destroy(self->group_array[i]);
    }

    for (i = 0; i < self->attr_group_count; i ++) {
        FREE_OBJECT(self->attr_group_array[i].attr_item_id_array);
    }
    FREE_OBJECT(self->attr_group_array);

    for (i = 0; i < self->attr_relation_count; i ++) {
        FREE_OBJECT(self->attr_relation_array[i].attr_item1_array);
        FREE_OBJECT(self->attr_relation_array[i].attr_item2_array);
    }
    FREE_OBJECT(self->attr_relation_array);

    for (i = 0; i < self->attr_id_name_count; i ++) {
        FREE_OBJECT(self->attr_id_name_array[i].name);
    }
    FREE_OBJECT(self->attr_id_name_array);

    for (i = 0; i < self->attr_item_id_name_count; i ++) {
        FREE_OBJECT(self->attr_item_id_name_array[i].name);
    }
    FREE_OBJECT(self->attr_item_id_name_array);

    for (i = 0; i < self->attr_checked_item_count; i ++) {
        FREE_OBJECT(self->attr_checked_item_array[i].attr_item_array);
    }
    FREE_OBJECT(self->attr_checked_item_array);

    for (i = 0; i < self->table_count; i ++) {
        tfd_table_destroy(self->table_array[i]);
    }
    FREE_OBJECT(self->table_array);

    FREE_OBJECT(self->group_array);
    FREE_OBJECT(self);
}

const tfd_attr_checked_item* tfd_manager_attr_checked_items(tfd_manager *self, u32 *array_count) {
    *array_count = self->attr_checked_item_count;
    return self->attr_checked_item_array;
}

void tfd_manager_write_filter(tfd_manager *self, tfd_filter *filter) {
    u32 i, j;
    tfd_filter_dump(filter);
    for (i = 0; i < self->header.group_count; i ++) {
        tfd_group *group = self->group_array[i];
        u32 module_count = tfd_group_module_count(group);
        for (j = 0; j < module_count; j ++) {
            tfd_module *module = tfd_group_module(group, j);
            tfd_module_write_filter(module, filter);
        }
    }
}

u32 tfd_manager_group_count(tfd_manager *self) {
    return self->header.group_count;
}

tfd_group* tfd_manager_group(tfd_manager *self, u32 group_index) {
    TV_ASSERT(group_index < self->header.group_count);
    return self->group_array[group_index];
}

tfd_module* tfd_manager_module(tfd_manager *self, tfd_module_id module_id) {
    tfd_group *group;
    tfd_group_id group_id = tfd_module_id_to_group_id(module_id);
    TV_ASSERT(group_id < self->header.group_count);
    group = self->group_array[group_id];
    return tfd_group_module(group, tfd_module_id_to_module_index(module_id));
}

tfd_table* tfd_manager_table(tfd_manager *self, u32 table_id) {
    TV_ASSERT(table_id < self->table_count);
    return self->table_array[table_id];
}

tfd_manager *tfd_manager_create(u8 *data, u32 size) {
    tfd_manager *self = MALLOC_OBJECT(tfd_manager);
    u32 i, j;
    u8 *offset = data;
    TV_MEMCPY(&self->header, offset, sizeof(self->header));
    offset += sizeof(self->header);

    /* load group/module/state */
    self->group_array = MALLOC_OBJECT_ARRAY(tfd_group*, self->header.group_count);

    for (i = 0; i < self->header.group_count; i ++) {
        self->group_array[i] = tfd_group_create(self, &offset);
    }

    /* load attribute group */
    self->attr_group_count = *(u32*)offset;
    offset += sizeof(u32);

    if (self->attr_group_count > 0) {
        self->attr_group_array = MALLOC_OBJECT_ARRAY(tfd_attr_group, self->attr_group_count);
        for (i = 0; i < self->attr_group_count; i ++) {
            tfd_attr_group *attr_group = &self->attr_group_array[i];
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

    /* load relationship */
    self->attr_relation_count = *(u32*)offset;
    offset += sizeof(u32);
    if (self->attr_relation_count > 0) {
        self->attr_relation_array = MALLOC_OBJECT_ARRAY(tfd_attr_relation, self->attr_relation_count);
        for (i = 0; i < self->attr_relation_count; i ++) {
            tfd_attr_relation *relation = &self->attr_relation_array[i];

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

    /* load attribute id => name */
    self->attr_id_name_count = *(u32*)offset;
    offset += sizeof(u32);
    self->attr_id_name_array = MALLOC_OBJECT_ARRAY(tfd_attr_id_name, self->attr_id_name_count);

    for (i = 0; i < self->attr_id_name_count; i ++) {
        u32 name_len;
        tfd_attr_id_name *attr_id_name = &self->attr_id_name_array[i];
        attr_id_name->attr_id = *(tfd_attr_id*)offset;
        offset += sizeof(tfd_attr_id);

        name_len = *(u8*)offset;
        offset += sizeof(u8);

        attr_id_name->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
        TV_MEMCPY(attr_id_name->name, offset, name_len);
        offset += name_len;
    }

    /* load attribute item id => name */
    self->attr_item_id_name_count = *(u32*)offset;
    offset += sizeof(u32);
    self->attr_item_id_name_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id_name, self->attr_item_id_name_count);

    for (i = 0; i < self->attr_item_id_name_count; i ++) {
        u32 name_len;
        tfd_attr_item_id_name *attr_item_id_name = &self->attr_item_id_name_array[i];
        attr_item_id_name->attr_item_id = *(tfd_attr_item_id*)offset;
        offset += sizeof(tfd_attr_item_id);

        name_len = *(u8*)offset;
        offset += sizeof(u8);

        attr_item_id_name->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
        TV_MEMCPY(attr_item_id_name->name, offset, name_len);
        offset += name_len;
    }
    /* load checked attribute item */
    self->attr_checked_item_count = *(u32*)offset;
    offset += sizeof(u32);
    self->attr_checked_item_array = MALLOC_OBJECT_ARRAY(tfd_attr_checked_item, self->attr_checked_item_count);
    for (i = 0; i < self->attr_checked_item_count; i ++) {
        tfd_attr_checked_item *checked_item = &self->attr_checked_item_array[i];

        checked_item->attr_id = *(tfd_attr_id*)offset;
        offset += sizeof(tfd_attr_id);

        checked_item->attr_item_count = *(u32*)offset;
        offset += sizeof(u32);

        checked_item->attr_item_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, checked_item->attr_item_count);
        TV_MEMCPY(checked_item->attr_item_array, offset, sizeof(tfd_attr_item_id) * checked_item->attr_item_count);
        offset += sizeof(tfd_attr_item_id) * checked_item->attr_item_count;
    }

    /* load table */
    self->table_count = *(u32*)offset;
    offset += sizeof(u32);
    self->table_array = MALLOC_OBJECT_ARRAY(tfd_table*, self->table_count);
    for (i = 0; i < self->table_count; i ++) {
        self->table_array[i] = tfd_table_create(self, &offset);
    }
    return self;
}

const char *tfd_manager_attr_name(tfd_manager *self, tfd_attr_id attr_id) {
    u32 i;
    for (i = 0; i < self->attr_id_name_count; i ++) {
        tfd_attr_id_name *id_name = &self->attr_id_name_array[i];
        if (id_name->attr_id == attr_id) {
            return id_name->name;
        }
    }
    return "";
}

const char *tfd_manager_attr_item_name(tfd_manager *self, tfd_attr_item_id attr_item_id) {
    u32 i;
    for (i = 0; i < self->attr_item_id_name_count; i ++) {
        tfd_attr_item_id_name *id_name = &self->attr_item_id_name_array[i];
        if (id_name->attr_item_id == attr_item_id) {
            return id_name->name;
        }
    }
    return "";
}
