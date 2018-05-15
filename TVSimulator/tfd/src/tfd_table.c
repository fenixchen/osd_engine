#include "tfd_internal.h"


struct _tfd_table_priv {
    u16 row_count;
    u16 field_count;
    u32 *field_type;
    u32 *field_attr;
    u32 *cells;
};

static void tfd_table_destroy(tfd_table *self) {
    FREE_OBJECT(self->priv->field_type);
    FREE_OBJECT(self->priv->field_attr);
    FREE_OBJECT(self->priv->cells);
    FREE_OBJECT(self->priv);
    FREE_OBJECT(self);
}


static u16 tfd_table_row_count(tfd_table *self) {
    return self->priv->row_count;
}

static u16 tfd_table_field_count(tfd_table *self) {
    return self->priv->field_count;
}

static u32 tfd_table_cell(tfd_table *self, u16 row, u16 field) {
    tfd_table_priv *priv = self->priv;
    TV_ASSERT(row < priv->row_count && field < priv->field_count);
    return priv->cells[priv->field_count * row + field];
}

tfd_table *tfd_table_create(tfd_manager *manager, const u8 **p_offset) {
    int i, j;
    tfd_table_priv *priv;
    const u8 *offset = *p_offset;
    tfd_table *self = MALLOC_OBJECT(tfd_table);
    self->priv = MALLOC_OBJECT(tfd_table_priv);
    priv = self->priv;
    self->destroy = tfd_table_destroy;
    self->row_count = tfd_table_row_count;
    self->field_count = tfd_table_field_count;
    self->cell = tfd_table_cell;
    TV_TYPE_FP_CHECK(self->destroy, self->cell);

    priv->field_count = *(u32*)offset;
    offset += sizeof(u32);

    priv->row_count = *(u32*)offset;
    offset += sizeof(u32);

    priv->field_type = MALLOC_OBJECT_ARRAY(u32, priv->field_count);
    priv->field_attr = MALLOC_OBJECT_ARRAY(u32, priv->field_count);
    for (i = 0; i < priv->field_count; i ++) {
        priv->field_type[i] = *(u32*)offset;
        offset += sizeof(u32);
        priv->field_attr[i] = *(u32*)offset;
        offset += sizeof(u32);
    }
    priv->cells = MALLOC_OBJECT_ARRAY(u32, priv->field_count * priv->row_count);
    for (i = 0; i < priv->row_count; i ++) {
        for (j = 0; j < priv->field_count; j ++) {
            priv->cells[priv->field_count * i + j] = *(u32*)offset;
            offset += sizeof(u32);
        }
    }
    *p_offset = offset;
    return self;
}
