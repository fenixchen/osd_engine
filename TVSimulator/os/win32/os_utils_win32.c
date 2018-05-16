#include <windows.h>
#include "os_utils.h"

void os_msleep(unsigned int ms) {
    Sleep(ms);
}

static void os_init_console() {
    /* support escape color text */
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
}


void os_init() {
    os_init_console();
}




