#include "PCH.hpp"
#include "Resource.h"
#include "TunkioProgressDialog.hpp"

namespace Tunkio
{
    ProgressDialog* g_instance = nullptr;

    std::wstring GetText(HWND window)
    {
        std::wstring buffer;
        int length = GetWindowTextLength(window);

        if (length)
        {
            buffer.resize(length);
            GetWindowText(window, &buffer.front(), length + 1);
        }

        return buffer;
    }

    std::wstring LoadStdString(HINSTANCE instance, UINT id)
    {
        std::wstring buffer(1, L'\0');
        int length = LoadString(instance, id, &buffer.front(), 0);

        if (length)
        {
            buffer.resize(length);
            LoadString(instance, id, &buffer.front(), length + 1);
        }

        return buffer;
    }

    ProgressDialog::ProgressDialog(HINSTANCE instance, const std::wstring& commandLine) :
        m_instance(instance),
        m_commandLine(commandLine)
    {
        g_instance = this;
    }

    ProgressDialog::~ProgressDialog()
    {
        g_instance = nullptr;
    }

    bool ProgressDialog::Show()
    {
        INT_PTR result = DialogBox(m_instance, MAKEINTRESOURCE(IDD_TUNKIO_PROGRESS_DIALOG), nullptr, DialogProcedure);

        if (result == IDOK)
        {
            return true;
        }

        return false;
    }

    INT_PTR CALLBACK ProgressDialog::DialogProcedure(HWND dialog, UINT message, WPARAM wParam, LPARAM lParam)
    {
        UNREFERENCED_PARAMETER(lParam);

        switch (message)
        {
            case WM_INITDIALOG:
            {
                HWND textBox = GetDlgItem(dialog, IDC_TUNKIO_PROGRESS_DIALOG_TEXT);
                SetWindowText(textBox, g_instance->m_commandLine.c_str());

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