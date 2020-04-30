#pragma once

#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
    // TODO: refactor cross-platform with pimpl idiom
    class ChildProcess
    {
    public:
        ChildProcess(const std::filesystem::path& path, const std::wstring& arguments);
        bool Run();

        uint32_t ErrorCode() const;
        uint32_t ExitCode() const;
        std::string Output() const;

    private:
        DWORD m_errorCode = 0;
        DWORD m_exitCode = 0;
        std::string m_stdout;

        bool InitializePipes();
        bool CreateChildProcess(PROCESS_INFORMATION& pi);
        void ReadFromPipe();

        const std::filesystem::path m_path;
        const std::wstring m_arguments;

        Win32AutoHandle m_stdoutRead;
        Win32AutoHandle m_stdoutWrite;
    };
}