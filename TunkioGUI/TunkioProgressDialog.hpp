#pragma once

#include <Windows.h>
#include <string>

namespace Tunkio
{
    class ProgressDialog
    {
    public:
        ProgressDialog(HINSTANCE instance, const std::wstring& commandLine);
        ~ProgressDialog();

        bool Show();

    private:
        static INT_PTR CALLBACK DialogProcedure(HWND, UINT, WPARAM, LPARAM);

        const HINSTANCE m_instance;
        const std::wstring m_commandLine;
    };
}