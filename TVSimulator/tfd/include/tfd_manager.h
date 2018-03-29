#ifndef _TFD_MANAGER_H
#define _TFD_MANAGER_H

#include "tfd_types.h"

typedef struct _tfd_manager_priv tfd_manager_priv;

typedef struct _tfd_manager tfd_manager;

struct _tfd_manager {
    tfd_manager_priv *priv;
    void (*destroy)(tfd_manager *self);
    t_bool (*load)(tfd_manager *self, u8 *data, u32 size);
};

EXTERNC tfd_manager *tfd_manager_create();


#endif