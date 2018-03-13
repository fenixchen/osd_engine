#ifndef _OSD_BINARY_H
#define _OSD_BINARY_H

#include "osd_types.h"

typedef struct _osd_binary_priv osd_binary_priv;

struct _osd_binary {
    osd_binary_priv *priv;
    void (*destroy)(osd_binary *self);
    u8* (*data)(osd_binary *self, u32 *size);
};

osd_binary *osd_binary_create(const char *target_folder);

#endif