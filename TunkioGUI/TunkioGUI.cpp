#include "PCH.hpp"
#include "Resource.h"
#include "TunkioGUI.hpp"

#define MAX_LOADSTRING 100

namespace Tunkio
{
    HINSTANCE g_instance = nullptr; // current instance
    WCHAR g_title[MAX_LOADSTRING] = { 0 }; // The title bar text
    WCHAR g_windowClassName[MAX_LOADSTRING] = { 0 }; // the main window class name

    void LoadStrings(HINSTANCE instance)
    {
        LoadString(instance, IDS_APP_TITLE, Tunkio::g_title, MAX_LOADSTRING);
        LoadString(instance, IDC_TUNKIOGUI, Tunkio::g_windowClassName, MAX_LOADSTRING);
    }

    ATOM MyRegisterClass(HINSTANCE instance)
    {
        WNDCLASSEXW windowClass = { 0 };

        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = WindowProcedure;
        windowClass.cbClsExtra = 0;
        windowClass.cbWndExtra = 0;
        windowClass.hInstance = instance;
        windowClass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_TUNKIOGUI));
        windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        windowClass.lpszMenuName = MAKEINTRESOURCEW(IDC_TUNKIOGUI);
        windowClass.lpszClassName = g_windowClassName;
        windowClass.hIconSm = LoadIcon(windowClass.hInstance, MAKEINTRESOURCE(IDI_TUNKIOGUI));

        return RegisterClassEx(&windowClass);
    }

    BOOL InitInstance(HINSTANCE instance, int showCommand)
    {
        g_instance = instance;

        HWND window = CreateWindow(
            g_windowClassName,
            g_title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            0,
            CW_USEDEFAULT,
            0,
            nullptr,
            nullptr,
            instance,
            nullptr);

        if (!window)
        {
            return FALSE;
        }

        ShowWindow(window, showCommand);
        UpdateWindow(window);

        return TRUE;
    }

    LRESULT CALLBACK WindowProcedure(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                // Parse the menu selections:
                switch (wmId)
                {
                    case IDM_ABOUT:
                        DialogBox(g_instance, MAKEINTRESOURCE(IDD_ABOUTBOX), window, About);
                        break;
                    case IDM_EXIT:
                        DestroyWindow(window);
                        break;
                    default:
                        return DefWindowProc(window, message, wParam, lParam);
                }

                break;
            }

            case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(window, &ps);
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
                FillRect(hdc, &ps.rcPaint, hBrush);
                DeleteObject(hBrush);
                EndPaint(window, &ps);

                break;
            }

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }
        }

        return DefWindowProc(window, message, wParam, lParam);
    }

    INT_PTR CALLBACK About(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);

        switch (message)
        {
            case WM_INITDIALOG:
            {
                return static_cast<INT_PTR>(TRUE);
            }
            case WM_COMMAND:
            {
                if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
                {
                    EndDialog(dialog, LOWORD(wParam));
                    return static_cast<INT_PTR>(TRUE);
                }
                break;
            }
        }

        return static_cast<INT_PTR>(FALSE);
    }
}

int APIENTRY wWinMain(
    _In_ HINSTANCE instance,
    _In_opt_ HINSTANCE previousInstance,
    _In_ LPWSTR commandLine,
    _In_ int showCommand)
{
    UNREFERENCED_PARAMETER(previousInstance);
    UNREFERENCED_PARAMETER(commandLine);

    // TODO: Place code here.

    Tunkio::LoadStrings(instance);

    Tunkio::MyRegisterClass(instance);

    // Perform application initialization:
    if (!Tunkio::InitInstance(instance, showCommand))
    {
        return FALSE;
    }

    HACCEL acceleratorTable = LoadAccelerators(instance, MAKEINTRESOURCE(IDC_TUNKIOGUI));

    MSG message = { 0 };

    // Main message loop:
    while (GetMessage(&message, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(message.hwnd, acceleratorTable, &message))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }

    return static_cast<int>(message.wParam);
}