#pragma once

#include "TunkioWin32AutoHandle.hpp"
#include "TunkioProcess.hpp"

namespace Tunkio
{
    class Win32ChildProcess : public IProcess<wchar_t>
    {
    public:
        Win32ChildProcess(const std::filesystem::path& executable, const std::wstring& arguments);
        bool Start() override;



    private:
        bool InitializePipes();
        bool CreateChildProcess(PROCESS_INFORMATION& pi);
        void ReadFromPipe();

        Win32AutoHandle m_stdoutRead;
        Win32AutoHandle m_stdoutWrite;
    };
}