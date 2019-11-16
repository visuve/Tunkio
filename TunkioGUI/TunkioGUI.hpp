#pragma once

#include <Windows.h>

namespace Tunkio
{
    void LoadStrings(HINSTANCE);
    ATOM MyRegisterClass(HINSTANCE);
    BOOL InitInstance(HINSTANCE, int);
    LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
    INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
}