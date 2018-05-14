#ifndef _TFD_TYPES_H
#define _TFD_TYPES_H


#include "tv_api.h"

typedef u16 tfd_attr_id;
typedef u32 tfd_attr_item_id;
typedef u32 tfd_group_id;
typedef u32 tfd_module_id;
typedef u32 tfd_state_id;
typedef u32 tfd_table_id;

#pragma pack(1)

#define TFD_DESC_LEN 47
typedef struct _tfd_header tfd_header;
struct _tfd_header {
    u16   header_len;		//2
    u16   version;			//2
    u32   group_count;		//4
    u32   build_time;		//4
    char  desc[TFD_DESC_LEN];    //47
    u8    compressed;		//1 /* 0 means uncompressed, 1 means compressed */
    u32   version_number;   //4
    u32   file_size;		//4
};

typedef struct _tfd_group_header tfd_group_header;
struct _tfd_group_header {
    u16				header_len;
    u16				version;
    u32				module_count;
    tfd_group_id    group_id;
};

typedef struct _tfd_module_header tfd_module_header;
struct _tfd_module_header {
    u16				header_len;
    u16				version;
    u32				state_count;
    tfd_module_id	module_id;
    u8				send_mode;
};

typedef struct _tfd_state_header tfd_state_header;
struct _tfd_state_header {
    u16				header_len;
    u16				version;
    tfd_state_id	module_id;
};

#pragma pack()


#endif