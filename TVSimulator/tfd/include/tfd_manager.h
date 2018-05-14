#ifndef _TFD_MANAGER_H
#define _TFD_MANAGER_H

#include "tfd_types.h"

typedef struct _tfd_manager_priv tfd_manager_priv;

typedef struct _tfd_manager tfd_manager;

struct _tfd_manager {
    tfd_manager_priv *priv;
    void (*destroy)(tfd_manager *self);
};

EXTERNC tfd_manager *tfd_manager_create(u8 *data, u32 size);


typedef struct _tfd_group_priv tfd_group_priv;

typedef struct _tfd_group tfd_group;

struct _tfd_group {
    tfd_group_priv *priv;
    void (*destroy)(tfd_group *self);
};


typedef struct _tfd_module_priv tfd_module_priv;
typedef struct _tfd_module tfd_module;
struct _tfd_module {
    tfd_module_priv *priv;
    void (*destroy)(tfd_module *self);
    void (*send)(tfd_module *self, u8 index);
};



#endif