#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <windowsx.h>
#include "D2DClass.h"

namespace {
    D2DClass d2d_class;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (RegisterClass(&wc) == 0) {
        return 1;
    }

    HWND desktop_window = GetDesktopWindow();
    RECT desktop_rc;
    GetWindowRect(desktop_window, &desktop_rc);

    // Create the window.
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        TEXT("Szachy"),    // Window text
        WS_POPUP,            // Window style

        // Size and position
        0, 0, desktop_rc.right, desktop_rc.bottom,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    BOOL ret;

    while ((ret = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (ret == -1) {
            return 1;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT_PTR IDT_TIMER1 = 1;
    switch (uMsg)
    {
    case WM_CREATE:
    {
        d2d_class.InitDirect2D(hwnd);
        SetTimer(hwnd, IDT_TIMER1, 10, (TIMERPROC)NULL);
        return 0;
    }
    case WM_DESTROY:
    {
        d2d_class.DestroyDirect2D();
        PostQuitMessage(0);
        KillTimer(hwnd, IDT_TIMER1);
        return 0;
    }
    case WM_PAINT:
    {
        d2d_class.OnPaint(hwnd);
        ValidateRect(hwnd, nullptr);
        return 0;
    }
    case WM_TIMER:
    {
        InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN:
    {
        FLOAT xPos = GET_X_LPARAM(lParam);
        FLOAT yPos = GET_Y_LPARAM(lParam);
        d2d_class.HandleMouseClick(xPos, yPos);
        return 0;
    }
    return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
