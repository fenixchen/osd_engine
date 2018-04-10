#include "os_api.h"


void *thread_func(void *param) {
    int i = 0;
    for (i = 0; i < 5; i ++) {
        TV_LOG("Hello %d\n", i);
        os_msleep(1000);
    }
    return NULL;
}

void os_thread_test() {
    os_thread *thread = os_thread_create(thread_func, NULL);
    os_thread_run(thread);
    os_thread_wait(thread);
    TV_TYPE_FREE(thread);
}


void os_mq_test() {
    u32 i;
    os_mq *mq = os_mq_create(3, 4);
    i = 1;
    mq->put(mq, &i);

    i = 2;
    mq->put(mq, &i);

    i = 3;
    mq->put(mq, &i);

    mq->put(mq, &i);

    TV_TYPE_FREE(mq);
}

void os_test() {
    //os_thread_test();
    os_mq_test();
}