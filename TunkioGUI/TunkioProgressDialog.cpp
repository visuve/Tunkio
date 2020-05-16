#include "PCH.hpp"
#include "Resource.h"
#include "TunkioProgressDialog.hpp"
#include "TunkioTime.hpp"
#include "TunkioDataUnits.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioArgs.hpp"
#include "TunkioMemory.hpp"

namespace Tunkio
{
    ProgressDialog* g_instance = nullptr;
    Time::Timer g_totalTimer;
    Time::Timer g_currentTimer;
    uint32_t g_error = ErrorCode::Success;
    uint64_t g_bytesToWrite = 0;
    uint64_t g_bytesWrittenLastTime = 0;

    std::map<std::string, Args::Argument> Arguments =
    {
        { "target", Args::Argument(false, TunkioTarget::File) },
        { "mode", Args::Argument(false, TunkioMode::Zeroes) },
        { "remove", Args::Argument(false, false) },
        { "path", Args::Argument(true, std::filesystem::path()) }
    };

    TunkioOptions* CreateOptions()
    {
        std::cout << std::setprecision(3) << std::fixed;

        const auto started = [](uint64_t bytesLeft) -> void
        {
            g_bytesToWrite = bytesLeft;
        };

        const auto progress = [](uint64_t bytesWritten) -> void
        {
            if (!bytesWritten)
            {
                return;
            }

            const auto elapsedSince = g_currentTimer.Elapsed<Time::MilliSeconds>();
            const auto elapsedTotal = g_totalTimer.Elapsed<Time::MilliSeconds>();
            const DataUnit::Byte bytesWrittenSince(bytesWritten - g_bytesWrittenLastTime);
            const DataUnit::Byte bytesWrittenTotal(bytesWritten);

            if (bytesWrittenTotal.Value() && elapsedSince.count())
            {
                const DataUnit::Byte bytesLeft = g_bytesToWrite - bytesWritten;
                std::cout << DataUnit::HumanReadable(bytesWrittenTotal) << " written."
                    << " Speed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince)
                    << ". Avg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal)
                    << ". Time left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, g_totalTimer))
                    << std::endl;
            }

            g_currentTimer.Reset();
            g_bytesWrittenLastTime = bytesWritten;
        };

        const auto errors = [](uint32_t error, uint64_t bytesWritten) -> void
        {
            if (bytesWritten)
            {
                std::cerr << "Error " << error << " occurred. Bytes written: " << bytesWritten << std::endl;
            }
            else
            {
                std::cerr << "Error " << error << " occurred." << std::endl;
            }

            std::string buffer(0x400, 0);
            if (FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr,
                error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                buffer.data(),
                static_cast<DWORD>(buffer.size()),
                nullptr))
            {
                std::cerr << "Detailed description: " << buffer << std::endl;
            }

            g_error = error;
        };

        const auto completed = [](uint64_t bytesWritten) -> void
        {
            std::cout << "Finished. Bytes written: " << bytesWritten << std::endl;

            const DataUnit::Byte bytes(bytesWritten);
            const auto elapsed = g_totalTimer.Elapsed<Time::MilliSeconds>();

            if (bytes.Value() && elapsed.count())
            {
                std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
            }
        };

        const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

        return new TunkioOptions
        {
            Arguments.at("target").Value<TunkioTarget>(),
            Arguments.at("mode").Value<TunkioMode>(),
            Arguments.at("remove").Value<bool>(),
            TunkioCallbacks { started, progress, errors, completed },
            TunkioString{ path.size(), Memory::CloneString(path) }
        };
    }

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