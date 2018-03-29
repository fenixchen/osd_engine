#include "osd_common.h"
#include "osd_binary.h"

struct _osd_binary_priv {
    u8 *binary;
    u32 binary_size;
};

static u8* osd_binary_data(osd_binary *self, u32 *size) {
    TV_TYPE_GET_PRIV(osd_binary_priv, self, priv);
    if (size) {
        *size = priv->binary_size;
    }
    return priv->binary;
}

static void osd_binary_destroy(osd_binary *self) {
    TV_TYPE_GET_PRIV(osd_binary_priv, self, priv);
    FREE_OBJECT(priv->binary);
    FREE_OBJECT(priv);
    FREE_OBJECT(self);
}

osd_binary *osd_binary_create(const char *osd_file) {
    osd_binary *self = MALLOC_OBJECT(osd_binary);
    osd_binary_priv *priv = MALLOC_OBJECT(osd_binary_priv);
    self->priv = priv;
    self->destroy = osd_binary_destroy;
    self->data = osd_binary_data;

    priv->binary = osd_read_file(osd_file, &priv->binary_size);
    if (!priv->binary) {
        self->destroy(self);
        return NULL;
    }
    return self;
}