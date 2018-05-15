#include "tfd_api.h"
#include "E:\\Soc\\KVTool\\Bin\\Database\\database.h"

void tfd_test() {
    u32 len, i;
    tfd_manager *mgr;
    u8 *data;

    data = tv_read_file("E:\\Soc\\KVTool\\Bin\\Database\\database.TSE", &len);
    TV_ASSERT(data);
    FREE_BUFFER(data);


    mgr = tfd_manager_create(data, len);
    {
        u32 group_count = mgr->group_count(mgr);
        for (i = 0; i < group_count; i ++) {
            tfd_group *group = mgr->group(mgr, i);
            TV_LOG("group:%s\n", group->name(group));
        }
    }

    mgr->destroy(mgr);
}