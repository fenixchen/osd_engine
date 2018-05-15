#include "tfd_internal.h"

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

typedef struct _tfd_attr_checked_item tfd_attr_checked_item;
struct _tfd_attr_checked_item {
    tfd_attr_id attr_id;
    u32 attr_item_count;
    tfd_attr_item_id *attr_item_array;
};

struct _tfd_manager_priv {
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

    for (i = 0; i < priv->attr_checked_item_count; i ++) {
        FREE_OBJECT(priv->attr_checked_item_array[i].attr_item_array);
    }
    FREE_OBJECT(priv->attr_checked_item_array);

    for (i = 0; i < priv->table_count; i ++) {
        priv->table_array[i]->destroy(priv->table_array[i]);
    }
    FREE_OBJECT(priv->table_array);

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

    /* load group/module/state */
    priv->group_array = MALLOC_OBJECT_ARRAY(tfd_group*, priv->header.group_count);

    for (i = 0; i < priv->header.group_count; i ++) {
        priv->group_array[i] = tfd_group_create(self, &offset);
    }

    /* load attribute group */
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

    /* load relationship */
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

    /* load attribute id => name */
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

    /* load attribute item id => name */
    priv->attr_item_id_name_count = *(u32*)offset;
    offset += sizeof(u32);
    priv->attr_item_id_name_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id_name, priv->attr_item_id_name_count);

    for (i = 0; i < priv->attr_item_id_name_count; i ++) {
        u32 name_len;
        tfd_attr_item_id_name *attr_item_id_name = &priv->attr_item_id_name_array[i];
        attr_item_id_name->attr_item_id = *(tfd_attr_item_id*)offset;
        offset += sizeof(tfd_attr_item_id);

        name_len = *(u8*)offset;
        offset += sizeof(u8);

        attr_item_id_name->name = MALLOC_OBJECT_ARRAY(char, name_len + 1);
        TV_MEMCPY(attr_item_id_name->name, offset, name_len);
        offset += name_len;
    }
    /* load checked attribute item */
    priv->attr_checked_item_count = *(u32*)offset;
    offset += sizeof(u32);
    priv->attr_checked_item_array = MALLOC_OBJECT_ARRAY(tfd_attr_checked_item, priv->attr_checked_item_count);
    for (i = 0; i < priv->attr_checked_item_count; i ++) {
        tfd_attr_checked_item *checked_item = &priv->attr_checked_item_array[i];

        checked_item->attr_id = *(tfd_attr_id*)offset;
        offset += sizeof(tfd_attr_id);

        checked_item->attr_item_count = *(u32*)offset;
        offset += sizeof(u32);

        checked_item->attr_item_array = MALLOC_OBJECT_ARRAY(tfd_attr_item_id, checked_item->attr_item_count);
        TV_MEMCPY(checked_item->attr_item_array, offset, sizeof(tfd_attr_item_id) * checked_item->attr_item_count);
        offset += sizeof(tfd_attr_item_id) * checked_item->attr_item_count;
    }

    /* load table */
    priv->table_count = *(u32*)offset;
    offset += sizeof(u32);
    priv->table_array = MALLOC_OBJECT_ARRAY(tfd_table*, priv->table_count);
    for (i = 0; i < priv->table_count; i ++) {
        priv->table_array[i] = tfd_table_create(self, &offset);
    }
}


static tfd_filter* tfd_manager_create_filter(tfd_manager *self) {
    return NULL;
}

static void tfd_manager_send(tfd_manager *self, tfd_filter *filter) {

}

static u32 tfd_manager_group_count(tfd_manager *self) {
    return 0;
}

static tfd_group* tfd_manager_group(tfd_manager *self, u32 group_index) {
    return NULL;
}

static tfd_module* tfd_manager_module(tfd_manager *self, tfd_module_id module_id) {
    return NULL;
}

tfd_manager *tfd_manager_create(u8 *data, u32 size) {
    tfd_manager *self = MALLOC_OBJECT(tfd_manager);
    self->priv = MALLOC_OBJECT(tfd_manager_priv);
    self->destroy = tfd_manager_destroy;
    self->create_filter = tfd_manager_create_filter;
    self->send = tfd_manager_send;
    self->group_count = tfd_manager_group_count;
    self->group = tfd_manager_group;
    self->module = tfd_manager_module;
    TV_TYPE_FP_CHECK(self->destroy, self->module);
    tfd_manager_load(self, data, size);
    return self;
}
