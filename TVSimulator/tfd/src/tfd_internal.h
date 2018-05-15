#ifndef _TFD_INTERNAL_H
#define _TFD_INTERNAL_H

#include "tfd_manager.h"


tfd_group *tfd_group_create(tfd_manager *manager, const u8 **p_offset);

tfd_module *tfd_module_create(tfd_manager *manager, const u8 **p_offset);

tfd_table *tfd_table_create(tfd_manager *manager, const u8 **p_offset);

#endif