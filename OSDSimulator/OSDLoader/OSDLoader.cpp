// OSDLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../OSDSimulator/osd_object.h"
#include <stdlib.h>

#define BIN_BASE_DIR "hello.generated/"
#define GLOBAL_FILE		BIN_BASE_DIR"global.bin"
#define PALETTES_FILE	BIN_BASE_DIR"palettes.bin"
#define WINDOWS_FILE	BIN_BASE_DIR"windows.bin"
#define INGREDIENT_FILE	BIN_BASE_DIR"ingredients.bin"
#define MODIFIERS_FILE	BIN_BASE_DIR"modifiers.bin"

unsigned char *read_file(const char *filename, unsigned int *len) {
    unsigned int length;
    unsigned char *buffer = NULL;
    FILE * fp = fopen(GLOBAL_FILE, "rb");
    if (fp == NULL) {
        fprintf(stderr, "open %s failed.\n", filename);
        return buffer;
    }
    fseek(fp, 0L, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (unsigned char *)malloc(length);
    if (fread(buffer, length, 1, fp) != 1) {
        fprintf(stderr, "read %s failed.\n", filename);
        free(buffer);
        return NULL;
    }
    *len = length;
    fclose(fp);
    return buffer;
}

int _tmain(int argc, _TCHAR* argv[]) {
    unsigned int len = 0, i;
    unsigned char *buf = read_file(GLOBAL_FILE, &len);
    if (buf) {
        printf("len:%d\n", len);
        for (i = 0; i < len; i ++)
            printf("%02X ", buf[i]);
    }
    getchar();
    return 0;
}

