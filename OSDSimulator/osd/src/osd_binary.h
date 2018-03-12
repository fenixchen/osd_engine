#ifndef _OSD_BINARY_H
#define _OSD_BINARY_H

#include "osd_types.h"

typedef struct _osd_binary_priv osd_binary_priv;

typedef enum _binary_type {
    BINARY_TYPE_GLOBAL = 0,
    BINARY_TYPE_PALETTE = 1,
    BINARY_TYPE_INGREDIENT = 2,
    BINARY_TYPE_WINDOW = 3,
    BINARY_TYPE_RAM = 4,
} binary_type;

struct _osd_binary {
    osd_binary_priv *priv;
    void (*destroy)(osd_binary *self);
    u32 (*data_size)(osd_binary *self, binary_type type);
    u8* (*data)(osd_binary *self, binary_type type);
};

osd_binary *osd_binary_create(const char *target_folder);

#endif