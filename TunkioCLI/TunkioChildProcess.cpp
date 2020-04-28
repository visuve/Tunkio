#include "PCH.hpp"
#include "TunkioChildProcess.hpp"
#include "TunkioTime.hpp"

namespace Tunkio
{
    bool WaitForProcess(const PROCESS_INFORMATION& pi, DWORD& exitCode, const Time::Seconds seconds = Time::Seconds(INFINITE))
    {
        if (WaitForSingleObject(pi.hProcess, static_cast<DWORD>(seconds.count())) != WAIT_OBJECT_0)
        {
            return false;
        }

        return GetExitCodeProcess(pi.hProcess, &exitCode);
    }

    ChildProcess::ChildProcess(const std::filesystem::path& path, const std::wstring& arguments) :
        m_path(path),
        m_arguments(arguments)
    {
    }

    bool ChildProcess::Run()
    {
        if (!InitializePipes())
        {
            m_errorCode = GetLastError();
            return false;
        }

        PROCESS_INFORMATION pi = { 0 };

        if (!CreateChildProcess(pi))
        {
            m_errorCode = GetLastError();
            return false;
        }

        bool result = WaitForProcess(pi, m_exitCode);

        m_stdoutWrite.Reset();
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        if (!result)
        {
            m_errorCode = GetLastError();
            return false;
        }

        ReadFromPipe();
        return true;
    }

    uint32_t ChildProcess::ErrorCode() const
    {
        return m_errorCode;
    }

    uint32_t ChildProcess::ExitCode() const
    {
        return m_exitCode;
    }

    std::string ChildProcess::Output() const
    {
        return m_stdout;
    }

    bool ChildProcess::InitializePipes()
    {
        SECURITY_ATTRIBUTES sa = { 0 };
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle = true;

        if (!CreatePipe(m_stdoutRead.PHandle(), m_stdoutWrite.PHandle(), &sa, 0))
        {
            return false;
        }

        if (!SetHandleInformation(m_stdoutRead.Handle(), HANDLE_FLAG_INHERIT, 0))
        {
            return false;
        }

        return true;
    }

    bool ChildProcess::CreateChildProcess(PROCESS_INFORMATION& pi)
    {
        STARTUPINFO si = { 0 };
        si.cb = sizeof(STARTUPINFO);
        si.hStdOutput = m_stdoutWrite.Handle();
        si.dwFlags |= STARTF_USESTDHANDLES;

        std::wstring commandLine = m_path.wstring() + L" " + m_arguments;

        return CreateProcess(m_path.c_str(), commandLine.data(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi);
    }

    void ChildProcess::ReadFromPipe()
    {
        constexpr DWORD bufferSize = 0x1000;

        DWORD bytesRead;
        char buffer[bufferSize] = { 0 };

        while(ReadFile(m_stdoutRead.Handle(), buffer, bufferSize, &bytesRead, nullptr) && bytesRead > 0)
        {
            m_stdout.append(buffer, bytesRead);
        }
    }
}