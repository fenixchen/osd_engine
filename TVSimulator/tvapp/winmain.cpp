// OSDSimulator.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Shlwapi.h"
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <Commdlg.h>
#include "winmain.h"
extern "C"
{
#include "tv_app.h"
}

#define TFD_TEST 0
#define OS_TEST 0

//#define STARTUP_OSD_FILE "..\\scenes\\label.osd"
#define STARTUP_OSD_FILE "..\\atv\\no_signal.osd"
//#define STARTUP_OSD_FILE "..\\atv\\system_settings.osd"
//#define STARTUP_OSD_FILE "..\\scenes\\tv.osd"
//#define STARTUP_OSD_FILE "..\\scenes\\monitor.osd"


#define SIMULATOR_VERSION "0.5"

#define MAX_LOADSTRING 100

tv_app *g_tv_app = NULL;


// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
    MSG msg;
    HACCEL hAccelTable;

    // 初始化全局字符串
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_TVAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TVAPP));

    // 主消息循环:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TVAPP));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_TVAPP);
    wcex.lpszClassName	= szWindowClass;
    wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hWnd;
    RECT rect;
    int width = 800;
    int height = 600;
    hInst = hInstance; // 将实例句柄存储在全局变量中

    GetClientRect(GetDesktopWindow(), &rect);
    rect.left = (rect.right/2) - (width/2);
    rect.top = (rect.bottom/2) - (height/2);

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        rect.left, rect.top,
                        width, height, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

static void AdjustWindow(HWND hWnd, int width, int height) {

    RECT rect;
    rect.left = rect.top = 0;
    rect.right = width - 1;
    rect.bottom = height - 1;
    DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    AdjustWindowRect(&rect, dwStyle, TRUE);
    SetWindowPos(hWnd, NULL, rect.left, rect.top,
                 rect.right - rect.left + 1,
                 rect.bottom - rect.top + 1,
                 SWP_NOMOVE);
    GetClientRect(hWnd, &rect);
}

void DoOpen(HWND hWnd, const char *osd_file) {
    char szFile[260];
    if (osd_file == NULL) {
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "OSD binary(*.osd)\0*.osd\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = NULL;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = "..\\scenes\\";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        if (!GetOpenFileName(&ofn)) {
            return;
        }
        osd_file = szFile;
    }
    if (g_tv_app->load(g_tv_app, osd_file)) {
        char text[256];
        osd_rect rect = g_tv_app->rect(g_tv_app);
        const char *title = g_tv_app->title(g_tv_app);
        sprintf(text, "OSDSimulator v%s - %s %d x %d",
                SIMULATOR_VERSION, title, rect.width, rect.height);
        SetWindowText(hWnd, text);
        KillTimer(hWnd, 0);
        int timer_interval = g_tv_app->timer_interval(g_tv_app);
        if (timer_interval > 0) {
            SetTimer(hWnd, 0, timer_interval, NULL);
        }
        AdjustWindow(hWnd, rect.width, rect.height);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}


void DoSaveFB(HWND hWnd) {
    if (!g_tv_app->active_scene(g_tv_app)) {
        ::MessageBox(hWnd, "Load scene first", "Information", MB_OK + MB_ICONINFORMATION);
        return;
    }
    OPENFILENAME ofn;

    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Frame Buffer Files (*.fb)\0*.fb\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "fb";

    if (!GetSaveFileName(&ofn)) {
        return;
    }
    u32 length = g_tv_app->rect(g_tv_app).width * g_tv_app->rect(g_tv_app).height * 4;
    u32 *framebuffer = new u32[length];
    g_tv_app->paint(g_tv_app, framebuffer);
    int fd = open(szFileName, O_CREAT | O_WRONLY | O_TRUNC);
    if (fd != -1) {
        write(fd, framebuffer, length);
        close(fd);
    } else {
        printf("write file failed.\n");
    }
    delete[] framebuffer;
}

void DoTimer(HWND hWnd) {
    if (g_tv_app->active_scene(g_tv_app)) {
        if (g_tv_app->trigger(g_tv_app, OSD_EVENT_TIMER, NULL)) {
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
}

void DoPaint(HWND hWnd, HDC hDC) {
    if (!g_tv_app->active_scene(g_tv_app)) {
        return;
    }
    //DWORD dwTick = GetTickCount();
    RECT rect;

    HDC hDCMem = CreateCompatibleDC(hDC);

    GetClientRect(hWnd, &rect);

    HBITMAP hBmp = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, hBmp);

    BITMAPINFOHEADER bmih;
    memset(&bmih, 0, sizeof(BITMAPINFOHEADER));
    bmih.biSize = sizeof(BITMAPINFOHEADER);
    bmih.biWidth= rect.right;
    bmih.biHeight= -rect.bottom;
    bmih.biSizeImage = 0;
    bmih.biBitCount = 32;
    bmih.biCompression = BI_RGB;
    bmih.biPlanes = 1;

    int width = g_tv_app->rect(g_tv_app).width;
    int height = g_tv_app->rect(g_tv_app).height;
    u32 length = width * height * 4;

    u32 *framebuffer = new u32[length];
    g_tv_app->paint(g_tv_app, framebuffer);
    int ret = SetDIBits(hDCMem,
                        hBmp,
                        0,
                        rect.bottom,
                        framebuffer,
                        (BITMAPINFO*)&bmih,
                        DIB_RGB_COLORS);

    delete[] framebuffer;

    BitBlt(hDC, 0, 0, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);

    SelectObject(hDCMem, hOldBmp);
    DeleteDC(hDCMem);
    DeleteObject(hBmp);
    //printf("DoPaint Used:%d ms\n", GetTickCount() - dwTick);
}

void SetStdOutToNewConsole() {
    int hConHandle;
    long lStdHandle;
    FILE *fp;

    // Allocate a console for this app
    AllocConsole();

    // Redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    fp = _fdopen(hConHandle, "w");
    *stdout = *fp;

    setvbuf(stdout, NULL, _IONBF, 0);
}

extern "C" void tfd_test();
extern "C" void os_test();

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message) {
    case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);
        // 分析菜单选择:
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDM_OPEN:
            DoOpen(hWnd, NULL);
            break;
        case IDM_SAVE_FB:
            DoSaveFB(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        DoPaint(hWnd, hdc);
        EndPaint(hWnd, &ps);
        break;
    case WM_TIMER:
        DoTimer(hWnd);
        break;
    case WM_DESTROY:
        g_tv_app->destroy(g_tv_app);
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (g_tv_app->active_scene(g_tv_app)) {
            osd_event_data trigger;
            osd_key key = OSD_KEY_NONE;
            switch (wParam) {
            case VK_DOWN:
                key = OSD_KEY_DOWN;
                break;
            case VK_UP:
                key = OSD_KEY_UP;
                break;
            case VK_LEFT:
                key = OSD_KEY_LEFT;
                break;
            case VK_RIGHT:
                key = OSD_KEY_RIGHT;
                break;
            case VK_RETURN:
            case VK_SPACE:
                key = OSD_KEY_ENTER;
                break;
            case VK_BACK:
            case VK_ESCAPE:
                key = OSD_KEY_BACK;
                break;
            case 'M':
            case 'm':
                key = OSD_KEY_MENU;
                break;
            }
            if (key != OSD_KEY_NONE) {
                trigger.data.keydown.key = key;
                if (g_tv_app->trigger(g_tv_app, OSD_EVENT_KEYDOWN, &trigger)) {
                    InvalidateRect(hWnd, NULL, FALSE);
                }
            }
        }
        break;
    case WM_CREATE:
        SetStdOutToNewConsole();
#if TFD_TEST == 1
        tfd_test();
#endif
#if OS_TEST == 1
        os_test();
#endif
        g_tv_app = tv_app_create();
#ifdef STARTUP_OSD_FILE
        DoOpen(hWnd, STARTUP_OSD_FILE);
#endif
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
