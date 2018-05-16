#ifndef _TFD_TYPES_H
#define _TFD_TYPES_H


#include "tv_api.h"

typedef u16 tfd_attr_id;
typedef u32 tfd_attr_item_id;
typedef u32 tfd_group_id;
typedef u32 tfd_module_id;
typedef u32 tfd_state_id;
typedef u32 tfd_table_id;

typedef struct _tfd_filter tfd_filter;
typedef struct _tfd_manager tfd_manager;
typedef struct _tfd_group tfd_group;
typedef struct _tfd_module tfd_module;
typedef struct _tfd_table tfd_table;
typedef struct _tfd_plugin_regtable tfd_plugin_regtable;

#define tfd_module_id_to_group_id(module_id) (((module_id) >> 12) & 0xFF)

#define tfd_module_id_to_module_index(module_id) ((module_id) & 0xFFF)

#define tfd_attr_item_id_to_attr_id(attr_item_id) (((attr_item_id) >> 16) & 0xFFF)


enum TFD_PLUGIN_TYPE {
    TFD_PLUGIN_REGTABLE = 0x00090007,
};


#pragma pack(1)

#define TFD_DESC_LEN 47
typedef struct _tfd_header tfd_header;
struct _tfd_header {
    u32   group_count;		//4
    u32   build_time;		//4
    char  desc[TFD_DESC_LEN];    //47
    u8    compressed;		//1 /* 0 means uncompressed, 1 means compressed */
    u32   version_number;   //4
    u32   file_size;		//4
};

typedef struct _tfd_group_header tfd_group_header;
struct _tfd_group_header {
    u32				module_count;
    tfd_group_id    group_id;
};

typedef struct _tfd_module_header tfd_module_header;
struct _tfd_module_header {
    tfd_module_id	module_id;
    u16				state_count;
    u8				send_mode;
};

#pragma pack()

#endif