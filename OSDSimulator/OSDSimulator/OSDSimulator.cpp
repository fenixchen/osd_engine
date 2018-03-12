// OSDSimulator.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "OSDSimulator.h"
#include "Shlwapi.h"
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#include "osd_scene.h"

#define MAX_LOADSTRING 100


#define LOAD_DIRECT 1
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
    LoadString(hInstance, IDC_OSDSIMULATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSDSIMULATOR));

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
    wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OSDSIMULATOR));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OSDSIMULATOR);
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

    hInst = hInstance; // 将实例句柄存储在全局变量中

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, 640, 480, NULL, NULL, hInstance, NULL);

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

static osd_scene *scene = NULL;



void DoOpen(HWND hWnd) {
#if LOAD_DIRECT == 1
    char szFile[260] = "hello.generated\\global.bin";
#else
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OSD binary(global.bin)\0global.bin\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = ".";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    // Display the Open dialog box.
    if (!GetOpenFileName(&ofn)) {
        return;
    }
#endif
    PathRemoveFileSpec(szFile);
    if (scene) {
        osd_scene_delete(scene);
        scene = NULL;
    }
    scene = osd_scene_new(szFile);
    if (scene) {
        AdjustWindow(hWnd, scene->hw->width, scene->hw->height);
    }
}

static COLORREF g_color = RGB(0xFF, 0x00, 0x00);
void DoTimer(HWND hWnd) {
    //MessageBox(hWnd, _T("Hello"), _T("World"), MB_OK);
    //g_color = g_color + 10;
    //KillTimer(hWnd, 0);
    //InvalidateRect(hWnd, NULL, FALSE);
    if (scene) {
        if (osd_scene_timer(scene)) {
            InvalidateRect(hWnd, NULL, FALSE);
        }
    }
}

void FnSetPixel(void *arg, int x, int y, u32 color) {
    HDC hdc = (HDC)arg;
    SetPixel(hdc, x, y, color);
}

void DoPaint(HWND hWnd, HDC hDC) {
    RECT rect;
    HBITMAP hBmpMem;
    HDC hDCMem = CreateCompatibleDC(hDC);
    GetClientRect(hWnd, &rect);
    // 创建一块指定大小的位图
    hBmpMem = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
    // 将该位图选入到内存DC中，默认是全黑色的
    HGDIOBJ hObj = SelectObject(hDCMem, hBmpMem);
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    FillRect(hDCMem, &rect, hBrush);
    DeleteObject(hBrush);
    if (scene) {
        osd_scene_paint(scene, 0, FnSetPixel, hDCMem);
    }
    /* 将双缓冲区图像复制到显示缓冲区 */
    BitBlt(hDC, 0, 0, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);

    /* 释放资源 */
    SelectObject(hDCMem, hObj);
    DeleteObject(hBmpMem);
    DeleteDC(hDCMem);
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

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
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
            DoOpen(hWnd);
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
        if (scene) {
            osd_scene_delete(scene);
            scene = NULL;
        }
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        SetStdOutToNewConsole();
        SetTimer(hWnd, 0, 200, NULL);
#if LOAD_DIRECT == 1
        DoOpen(hWnd);
#endif
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
