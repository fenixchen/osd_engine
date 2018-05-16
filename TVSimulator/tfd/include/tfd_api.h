#ifndef _TFD_API_H
#define _TFD_API_H

#include "tfd_types.h"

/************************************************************************/
/*  TFD RegTable APIs													*/
/************************************************************************/
tfd_plugin_regtable *tfd_plugin_regtable_create(const u8 *data, u32 data_size);
void tfd_plugin_regtable_send(tfd_plugin_regtable *self, u32 state_index);
void tfd_plugin_regtable_destroy(tfd_plugin_regtable *self);

/************************************************************************/
/*  TFD filter APIs														*/
/************************************************************************/
tfd_filter *tfd_filter_create(tfd_manager *manager);
void tfd_filter_destroy(tfd_filter *self);
void tfd_filter_set(tfd_filter *self, tfd_attr_id attr_id, tfd_attr_item_id attr_item_id);
boolean tfd_filter_get(tfd_filter *self, tfd_attr_id attr_id, tfd_attr_item_id *attr_item_id);
u32 tfd_filter_count(tfd_filter *self);
void tfd_filter_attr_item(tfd_filter *self, u32 index, tfd_attr_id *attr_id, tfd_attr_item_id *attr_item_id);
void tfd_filter_dump(tfd_filter *self);
/************************************************************************/
/*  TFD group APIs														*/
/************************************************************************/
tfd_group *tfd_group_create(tfd_manager *manager, const u8 **p_offset);
void tfd_group_destroy(tfd_group *self);
const char * tfd_group_name(tfd_group *self);
u32 tfd_group_module_count(tfd_group *self);
tfd_module* tfd_group_module(tfd_group *self, u32 module_index);

/************************************************************************/
/*  TFD module APIs														*/
/************************************************************************/
tfd_module *tfd_module_create(tfd_manager *manager, const u8 **p_offset);
void tfd_module_destroy(tfd_module *self);
const char * tfd_module_name(tfd_module *self);
u32 tfd_module_state_count(tfd_module *self);
void tfd_module_write(tfd_module *self, u32 state_index);
void tfd_module_write_filter(tfd_module *self, tfd_filter *filter);

/************************************************************************/
/*  TFD table APIs														*/
/************************************************************************/
typedef enum _tfd_table_field_type tfd_table_field_type;
enum tfd_table_field_type {
    TFD_TABLE_FIELD_ATTR 	= 0,
    TFD_TABLE_FIELD_TABLE 	= 1,
    TFD_TABLE_FIELD_MODULE 	= 2,
    TFD_TABLE_FIELD_STATE 	= 3,
    TFD_TABLE_FIELD_INTEGER = 4,
    TFD_TABLE_FIELD_STRING  = 5,
};
tfd_table *tfd_table_create(tfd_manager *manager, const u8 **p_offset);
void tfd_table_destroy(tfd_table *self);
u16 tfd_table_row_count(tfd_table *self);
u16 tfd_table_field_count(tfd_table *self);
void tfd_table_field_info(tfd_table *self, u16 field_index, tfd_table_field_type *type, u32 *attr);
u32 tfd_table_cell(tfd_table *self, u16 row, u16 field);

/************************************************************************/
/*  TFD manager APIs                                                    */
/************************************************************************/
tfd_manager *tfd_manager_create(u8 *data, u32 size);
void tfd_manager_destroy(tfd_manager *self);
tfd_filter* tfd_manager_create_filter(tfd_manager *self);
void tfd_manager_write_filter(tfd_manager *self, tfd_filter *filter);
u32 tfd_manager_group_count(tfd_manager *self);
tfd_group* tfd_manager_group(tfd_manager *self, u32 group_index);
tfd_module* tfd_manager_module(tfd_manager *self, tfd_module_id module_id);
tfd_table* tfd_manager_table(tfd_manager *self, u32 table_id);
typedef struct _tfd_attr_checked_item tfd_attr_checked_item;
struct _tfd_attr_checked_item {
    tfd_attr_id attr_id;
    u32 attr_item_count;
    tfd_attr_item_id *attr_item_array;
};

const tfd_attr_checked_item* tfd_manager_attr_checked_items(tfd_manager *self, u32 *array_count);
const char *tfd_manager_attr_name(tfd_manager *self, tfd_attr_id attr_id);
const char *tfd_manager_attr_item_name(tfd_manager *self, tfd_attr_item_id attr_item_id);

#endif