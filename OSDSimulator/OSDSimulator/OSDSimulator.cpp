// OSDSimulator.cpp : ����Ӧ�ó������ڵ㡣
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


// ȫ�ֱ���:
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

// �˴���ģ���а����ĺ�����ǰ������:
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

    // TODO: �ڴ˷��ô��롣
    MSG msg;
    HACCEL hAccelTable;

    // ��ʼ��ȫ���ַ���
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_OSDSIMULATOR, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OSDSIMULATOR));

    // ����Ϣѭ��:
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
//  ע��:
//
//    ����ϣ��
//    �˴�������ӵ� Windows 95 �еġ�RegisterClassEx��
//    ����֮ǰ�� Win32 ϵͳ����ʱ������Ҫ�˺��������÷������ô˺���ʮ����Ҫ��
//    ����Ӧ�ó���Ϳ��Ի�ù�����
//    ����ʽ��ȷ�ġ�Сͼ�ꡣ
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
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��:
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    HWND hWnd;

    hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

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

void DoOpen(HWND hWnd, const char *osd_file) {
    char szFile[260];       // buffer for file name
    if (osd_file == NULL) {
        OPENFILENAME ofn;       // common dialog box structure
        // Initialize OPENFILENAME
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
        ofn.lpstrInitialDir = ".";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        // Display the Open dialog box.
        if (!GetOpenFileName(&ofn)) {
            return;
        }
        osd_file = szFile;
    }
    if (scene) {
        scene->destroy(scene);
        scene = NULL;
    }
    scene = osd_scene_create(osd_file);
    if (scene) {
        char title[256];
        sprintf(title, "OSDSimulator - %s", scene->title(scene));
        SetWindowText(hWnd, title);
        KillTimer(hWnd, 0);

        if (scene->timer_ms(scene) != 0) {
            SetTimer(hWnd, 0, scene->timer_ms(scene), NULL);
        }
        osd_rect rect = scene->rect(scene);
        AdjustWindow(hWnd, rect.width, rect.height);
    }
    InvalidateRect(hWnd, NULL, TRUE);
}

void DoTimer(HWND hWnd) {
    if (scene) {
        if (scene->timer(scene)) {
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
    // ����һ��ָ����С��λͼ
    hBmpMem = CreateCompatibleBitmap(hDC, rect.right, rect.bottom);
    // ����λͼѡ�뵽�ڴ�DC�У�Ĭ����ȫ��ɫ��
    HGDIOBJ hObj = SelectObject(hDCMem, hBmpMem);
    HBRUSH hBrush = CreateSolidBrush(RGB(0,0,0));
    FillRect(hDCMem, &rect, hBrush);
    DeleteObject(hBrush);
    if (scene) {
        scene->paint(scene, FnSetPixel, hDCMem);
    }
    /* ��˫������ͼ���Ƶ���ʾ������ */
    BitBlt(hDC, 0, 0, rect.right, rect.bottom, hDCMem, 0, 0, SRCCOPY);

    /* �ͷ���Դ */
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
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��: ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
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
        // �����˵�ѡ��:
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
            scene->destroy(scene);
            scene = NULL;
        }
        PostQuitMessage(0);
        break;
    case WM_CREATE:
        SetStdOutToNewConsole();
        DoOpen(hWnd, "test.osd");
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
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
