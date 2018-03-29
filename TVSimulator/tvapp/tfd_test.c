#include "tfd_api.h"


void tfd_test() {
    tfd_manager *mgr = tfd_manager_create();
    mgr->destroy(mgr);
}