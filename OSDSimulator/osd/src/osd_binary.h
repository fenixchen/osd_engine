#ifndef _OSD_BINARY_H
#define _OSD_BINARY_H

#include "osd_types.h"


osd_binary *osd_binary_new(const char *target_folder);

void osd_binary_delete(osd_binary *binary);

#endif