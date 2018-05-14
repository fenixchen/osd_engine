#include "tfd_api.h"


void tfd_test() {
    u32 len;
    tfd_manager *mgr;
    u8 *data;

    data = tv_read_file("E:\\Soc\\KVTool\\Bin\\Database\\database.TSE", &len);
    TV_ASSERT(data);

    mgr = tfd_manager_create(data, len);
    mgr->destroy(mgr);

    FREE_BUFFER(data);
}