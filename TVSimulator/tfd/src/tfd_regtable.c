#include "tfd_api.h"




#pragma pack(1)
typedef struct _page_reg_info page_reg_info;
struct _page_reg_info {
    u8 ucPageType;
    u8 ucRegAddrSize;
    u8 ucRegDataSize;
    u8 ucPageData[6];
    u32 dwRegCount;
    u8 *pRegAddr;
    u8 *pRegMask;
    u8 *pRegValue; //dwRegCount * state_count
};
#pragma pack()


struct _tfd_plugin_regtable {
    u8 multimask;
    u32 state_count;
    u32 page_count;
    page_reg_info *page_reg_info_array;
};

void tfd_plugin_regtable_destroy(tfd_plugin_regtable *self) {
    u32 i;
    for (i = 0; i < self->page_count; i ++) {
        page_reg_info *info = &self->page_reg_info_array[i];
        FREE_OBJECT(info->pRegAddr);
        FREE_OBJECT(info->pRegMask);
        FREE_OBJECT(info->pRegValue);
    }
    FREE_OBJECT(self->page_reg_info_array);
    FREE_OBJECT(self);
}
void tfd_plugin_regtable_send(tfd_plugin_regtable *self, u32 state_index) {
    u32 i, j;
    u32 base_address;
    u8 *reg_addr, *reg_mask, *reg_val;
    u32 addr, mask, val;
    static u32 s_full_mask_array[5] = {0, 0xFF, 0xFFFF, 0xFFFFFF, 0xFFFFFFFF,};

    for (i = 0; i < self->page_count; i ++) {
        u32 full_mask;
        page_reg_info *info = &self->page_reg_info_array[i];
        TV_ASSERT(info->ucRegDataSize <= 4);
        full_mask = s_full_mask_array[info->ucRegDataSize];

        reg_addr = info->pRegAddr;
        if (self->multimask) {
            reg_mask = info->pRegMask + state_index * info->ucRegDataSize;
        } else {
            reg_mask = info->pRegMask;
        }
        reg_val = info->pRegValue + state_index * info->ucRegDataSize;
        switch (info->ucPageType) {
        case 0:
            base_address = ((u32)info->ucPageData[0] << 24) |
                           ((u32)info->ucPageData[1] << 16) |
                           ((u32)info->ucPageData[2] << 8);

            for (j = 0; j < info->dwRegCount; j ++) {
                addr = mask = val = 0;
                TV_MEMCPY(&addr, reg_addr, info->ucRegAddrSize);
                TV_MEMCPY(&mask, reg_mask, info->ucRegDataSize);
                TV_MEMCPY(&val, reg_val, info->ucRegDataSize);
                if (self->multimask) {
                    reg_mask += info->ucRegDataSize * self->state_count;
                } else {
                    reg_mask += info->ucRegDataSize;
                }
                reg_addr += info->ucRegAddrSize;
                reg_val += info->ucRegDataSize * self->state_count;
                if (mask != full_mask) {
                    TV_LOGI("WriteMask(0x%08X, 0x%08X, 0x%08X)\n", base_address + info->ucRegDataSize * addr, mask, val);
                } else {
                    TV_LOGI("Write(0x%08X, 0x%08X)\n", base_address + info->ucRegDataSize * addr, val);
                }
            }
            break;
        default:
            TV_LOGE("Unknown page type:%d\n", info->ucPageType);
            TV_ASSERT(0);
        }
    }
}


tfd_plugin_regtable *tfd_plugin_regtable_create(const u8 *data, u32 data_size) {
    u32 i;
    const u8 *offset = data;
    u32 page_info_size = TV_OFFSET_OF(page_reg_info, pRegAddr);

    tfd_plugin_regtable *self = MALLOC_OBJECT(tfd_plugin_regtable);

    self->multimask = (*(u32*)offset == 1) ? 1 : 0;
    offset += sizeof(u32);

    self->state_count = *(u32*)offset;
    offset += sizeof(u32);

    self->page_count = *(u32*)offset;
    offset += sizeof(u32);

    self->page_reg_info_array = MALLOC_OBJECT_ARRAY(page_reg_info, self->page_count);

    for (i = 0; i < self->page_count; i ++) {
        u32 size;
        page_reg_info *info = &self->page_reg_info_array[i];
        TV_MEMCPY(info, offset, page_info_size);
        offset += page_info_size;

        size = info->ucRegAddrSize * info->dwRegCount;

        info->pRegAddr = MALLOC_OBJECT_ARRAY(u8, size);
        TV_MEMCPY(info->pRegAddr, offset, size);
        offset += size;

        if (self->multimask) {
            size = info->ucRegDataSize * info->dwRegCount * self->state_count;
        } else {
            size = info->ucRegDataSize * info->dwRegCount;
        }
        info->pRegMask = MALLOC_OBJECT_ARRAY(u8, size);
        TV_MEMCPY(info->pRegMask, offset, size);
        offset += size;

        size = info->ucRegDataSize * info->dwRegCount * self->state_count;
        info->pRegValue = MALLOC_OBJECT_ARRAY(u8, size);
        TV_MEMCPY(info->pRegValue, offset, size);
        offset += size;
    }
    return self;
}
