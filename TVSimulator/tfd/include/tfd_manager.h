#ifndef _TFD_MANAGER_H
#define _TFD_MANAGER_H

#include "tfd_types.h"

typedef struct _tfd_filter_priv tfd_filter_priv;
typedef struct _tfd_filter tfd_filter;

typedef struct _tfd_manager_priv tfd_manager_priv;
typedef struct _tfd_manager tfd_manager;

typedef struct _tfd_group_priv tfd_group_priv;
typedef struct _tfd_group tfd_group;

typedef struct _tfd_module_priv tfd_module_priv;
typedef struct _tfd_module tfd_module;

typedef struct _tfd_table_priv tfd_table_priv;
typedef struct _tfd_table tfd_table;

struct _tfd_filter {
    tfd_filter_priv *priv;
    void (*destroy)(tfd_filter *self);
    void (*set)(tfd_attr_id attr_id, tfd_attr_item_id attr_item_id);
    tfd_attr_item_id (*get)(tfd_attr_id attr_id);
};


struct _tfd_group {
    tfd_group_priv *priv;
    void (*destroy)(tfd_group *self);
    const char * (*name)(tfd_group *self);
    u32 (*module_count)(tfd_group *self);
    tfd_module* (*module)(tfd_group *self, u32 module_index);
};

struct _tfd_module {
    tfd_module_priv *priv;
    void (*destroy)(tfd_module *self);
    const char * (*name)(tfd_module *self);
    u32 (*state_count)(tfd_module *self);
    void (*send)(tfd_module *self, u32 state_index);
};

struct _tfd_table {
    tfd_table_priv *priv;
    void (*destroy)(tfd_table *self);
    u16 (*row_count)(tfd_table *self);
    u16 (*field_count)(tfd_table *self);
    u32 (*cell)(tfd_table *self, u16 row, u16 field);
};

struct _tfd_manager {
    tfd_manager_priv *priv;
    void (*destroy)(tfd_manager *self);
    tfd_filter* (*create_filter)(tfd_manager *self);
    void (*send)(tfd_manager *self, tfd_filter *filter);
    u32 (*group_count)(tfd_manager *self);
    tfd_group* (*group)(tfd_manager *self, u32 group_index);
    tfd_module* (*module)(tfd_manager *self, tfd_module_id module_id);
    u32 (*table_count)(tfd_manager *self);
    tfd_table (*table)(tfd_manager *self, u32 table_index);
};

EXTERNC tfd_manager *tfd_manager_create(u8 *data, u32 size);

#endif