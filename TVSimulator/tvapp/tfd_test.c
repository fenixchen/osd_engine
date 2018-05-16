#include "tfd_api.h"
#include "E:\\Soc\\KVTool\\Bin\\Database\\database.h"


void tfd_test() {
    u32 len, i, j;
    tfd_manager *mgr;
    u8 *data;
    TV_LOGV("TFD Test\n");
    data = tv_read_file("E:\\Soc\\KVTool\\Bin\\Database\\database.TSE", &len);
    TV_ASSERT(data);

    mgr = tfd_manager_create(data, len);

    {
        u32 group_count = tfd_manager_group_count(mgr);
        TV_LOGI("GROUP_COUNT:%d\n", group_count);
        for (i = 0; i < group_count; i ++) {
            tfd_group *group = tfd_manager_group(mgr, i);
            u32 module_count = tfd_group_module_count(group);
            TV_LOGI("GROUP:%s\n", tfd_group_name(group));
            for (j = 0; j < module_count; j ++) {
                tfd_module *module = tfd_group_module(group, j);
                TV_LOGI("\tMODULE:%s\n", tfd_module_name(module));
            }
        }
    }


    {
        u32 row_count, field_count;
        tfd_table *table = tfd_manager_table(mgr, TAB_MODULE);
        TV_ASSERT(table);
        row_count = tfd_table_row_count(table);
        field_count = tfd_table_field_count(table);
        TV_LOGI("TAB_MODULE[%d x %d]\n", row_count, field_count);
        for (i = 0; i < field_count; i ++) {
            tfd_table_field_type field_type;
            u32 field_attr;
            tfd_table_field_info(table, i, &field_type, &field_attr);
            TV_LOGI("\tFIELD[%d], type:%08X, attr:%08X\n", i, field_type, field_attr);
        }
        if (row_count > 0) {
            tfd_module *module;
            tfd_module_id module_id = tfd_table_cell(table, 0, 0);
            TV_LOGI("MODULE_ID:%08x\n", module_id);
            module = tfd_manager_module(mgr, module_id);
            TV_ASSERT(module);
            TV_LOGI("STATE_COUNT:%d\n", tfd_module_state_count(module));
            tfd_module_write(module, 0);
        }
    }

    {
        tfd_filter *filter = tfd_filter_create(mgr);
        tfd_filter_set(filter, AT_CHANNEL, AI_CHANNEL_CVBS);

        tfd_manager_write_filter(mgr, filter);
        tfd_filter_destroy(filter);
    }


    tfd_manager_destroy(mgr);

    FREE_BUFFER(data);
}