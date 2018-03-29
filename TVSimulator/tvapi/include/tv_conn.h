#ifndef _TV_CONN_H
#define _TV_CONN_H

#include "tv_types.h"

typedef struct _tv_conn_priv tv_conn_priv;

typedef struct _tv_conn tv_conn;

struct _tv_conn {
    tv_conn_priv *priv;
    void		(*destroy)(tv_conn *self);
    t_bool		(*set_config)(tv_conn *self, u32 param, u32 value);
    t_bool		(*get_config)(tv_conn *self, u32 param, u32 *value);
    t_bool		(*open)(tv_conn *self);
    t_bool		(*w32)(tv_conn *self, u32 addr, u32 value);
    t_bool		(*r32)(tv_conn *self, u32 addr, u32 *value);
    void		(*close)(tv_conn *self);
};

EXTERNC tv_conn *tv_conn_create();

#endif

