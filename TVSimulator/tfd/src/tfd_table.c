#include "tfd_api.h"

struct _tfd_table {
    u16 row_count;
    u16 field_count;
    u32 *field_type;
    u32 *field_attr;
    u32 *cells;
};

void tfd_table_destroy(tfd_table *self) {
    FREE_OBJECT(self->field_type);
    FREE_OBJECT(self->field_attr);
    FREE_OBJECT(self->cells);
    FREE_OBJECT(self);
}


u16 tfd_table_row_count(tfd_table *self) {
    return self->row_count;
}

u16 tfd_table_field_count(tfd_table *self) {
    return self->field_count;
}

void tfd_table_field_info(tfd_table *self, u16 field_index, tfd_table_field_type *type, u32 *attr) {
    TV_ASSERT(field_index < self->field_count);
    *type = (tfd_table_field_type)self->field_type[field_index];
    *attr = self->field_attr[field_index];
}

u32 tfd_table_cell(tfd_table *self, u16 row, u16 field) {
    TV_ASSERT(row < self->row_count && field < self->field_count);
    return self->cells[self->field_count * row + field];
}

tfd_table *tfd_table_create(tfd_manager *manager, const u8 **p_offset) {
    int i, j;
    const u8 *offset = *p_offset;
    tfd_table *self = MALLOC_OBJECT(tfd_table);

    self->field_count = *(u32*)offset;
    offset += sizeof(u32);

    self->row_count = *(u32*)offset;
    offset += sizeof(u32);

    self->field_type = MALLOC_OBJECT_ARRAY(u32, self->field_count);
    self->field_attr = MALLOC_OBJECT_ARRAY(u32, self->field_count);
    for (i = 0; i < self->field_count; i ++) {
        self->field_type[i] = *(u32*)offset;
        offset += sizeof(u32);
        self->field_attr[i] = *(u32*)offset;
        offset += sizeof(u32);
    }
    self->cells = MALLOC_OBJECT_ARRAY(u32, self->field_count * self->row_count);
    for (i = 0; i < self->row_count; i ++) {
        for (j = 0; j < self->field_count; j ++) {
            self->cells[self->field_count * i + j] = *(u32*)offset;
            offset += sizeof(u32);
        }
    }
    *p_offset = offset;
    return self;
}
