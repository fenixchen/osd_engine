#ifndef _OSD_BLOCK_H
#define _OSD_BLOCK_H

#include "osd_types.h"

typedef struct _osd_block_priv osd_block_priv;

struct _osd_block {
    osd_block_priv *priv;
    void		  (*destroy)(osd_block *self);
    u16			  (*ingredient_index)(osd_block *self);
    int			  (*visible)(osd_block *self);
    void		  (*set_visible)(osd_block *self, int visible);
    osd_point	  (*position)(osd_block *self);
    void		  (*set_position)(osd_block *self, osd_point *point);
    osd_block_hw* (*hw)(osd_block *self);
    void		  (*dump)(osd_block *self);
};

osd_block *osd_block_create(osd_scene *scene, osd_block_hw *hw);

#endif