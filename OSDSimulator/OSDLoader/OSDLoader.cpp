// OSDLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../OSDSimulator/osd_object.h"
#include <stdlib.h>




int _tmain(int argc, _TCHAR* argv[]) {
    osd_scene *scene = osd_scene_new("hello.generated");
    osd_scene_delete(scene);
    /*
    unsigned int len = 0, i;
    unsigned char *buf = read_file(GLOBAL_FILE, &len);
    if (buf) {
        printf("len:%d\n", len);
        for (i = 0; i < len; i ++)
            printf("%02X ", buf[i]);
    }
    */
    printf("Press ENTER to exit\n");
    getchar();
    return 0;
}

