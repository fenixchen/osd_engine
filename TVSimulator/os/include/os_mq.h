#ifndef _OS_MQ_H
#define _OS_MQ_H

#include "tv_types.h"

typedef struct _os_mq os_mq;

typedef struct _os_mq_priv os_mq_priv;

struct _os_mq {
    os_mq_priv *priv;
    void  (*destroy)(os_mq *self);
    void  (*put)(os_mq *self, void *data);
    void* (*get)(os_mq *self);
    int	  (*full)(os_mq *self);
    int	  (*empty)(os_mq *self);
};

os_mq *os_mq_create(u32 capacity, u32 data_size);

#endif