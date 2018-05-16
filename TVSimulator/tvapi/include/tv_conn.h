#ifndef _TV_CONN_H
#define _TV_CONN_H

#include "tv_types.h"

typedef struct _tv_conn_priv tv_conn_priv;

typedef struct _tv_conn tv_conn;

struct _tv_conn {
    tv_conn_priv *priv;
    void		(*destroy)(tv_conn *self);
    boolean		(*set_config)(tv_conn *self, u32 param, u32 value);
    boolean		(*get_config)(tv_conn *self, u32 param, u32 *value);
    boolean		(*open)(tv_conn *self);
    boolean		(*w32)(tv_conn *self, u32 addr, u32 value);
    boolean		(*r32)(tv_conn *self, u32 addr, u32 *value);
    void		(*close)(tv_conn *self);
};

EXTERNC tv_conn *tv_conn_create();

#endif

