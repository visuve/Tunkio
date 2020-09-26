#include "../PCH.hpp"
#include "../TunkioTime.hpp"
#include "TunkioWin32ChildProcess.hpp"

namespace Tunkio
{
	bool WaitForProcess(const PROCESS_INFORMATION& pi, uint32_t& exitCode, const Time::Seconds seconds = Time::Seconds(INFINITE))
	{
		if (WaitForSingleObject(pi.hProcess, static_cast<DWORD>(seconds.count())) != WAIT_OBJECT_0)
		{
			return false;
		}

		DWORD buffer = 0;

		if (GetExitCodeProcess(pi.hProcess, &buffer))
		{
			exitCode = buffer;
			return true;
		}

		return false;
	}

	Win32ChildProcess::Win32ChildProcess(const std::filesystem::path& executable, const std::wstring& arguments) :
		IProcess(executable, arguments)
	{
	}

	bool Win32ChildProcess::Start()
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

		if (pi.hProcess)
		{
			CloseHandle(pi.hProcess);
		}
		
		if (pi.hThread)
		{
			CloseHandle(pi.hThread);
		}

		if (!result)
		{
			m_errorCode = GetLastError();
			return false;
		}

		ReadFromPipe();
		return true;
	}

	bool Win32ChildProcess::InitializePipes()
	{
		SECURITY_ATTRIBUTES sa = { 0 };
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = true;

		if (!CreatePipe(m_stdoutRead.PHandle(), m_stdoutWrite.PHandle(), &sa, 0))
		{
			return false;
		}

		if (!SetHandleInformation(m_stdoutRead.Value(), HANDLE_FLAG_INHERIT, 0))
		{
			return false;
		}

		return true;
	}

	bool Win32ChildProcess::CreateChildProcess(PROCESS_INFORMATION& pi)
	{
		STARTUPINFOW si = { 0 };
		si.cb = sizeof(STARTUPINFOW);
		si.hStdOutput = m_stdoutWrite.Value();
		si.dwFlags |= STARTF_USESTDHANDLES;

		std::wstring commandLine = m_executable.wstring() + L" " + m_arguments;

		return CreateProcessW(
			m_executable.c_str(), commandLine.data(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi);
	}

	void Win32ChildProcess::ReadFromPipe()
	{
		DWORD bytesRead;
		std::array<char, 0xF> buffer;
		std::stringstream stream;

		while (ReadFile(
			m_stdoutRead.Value(), buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr) && bytesRead > 0)
		{
			stream.write(buffer.data(), bytesRead);
		}

		std::string line;

		while (std::getline(stream, line))
		{
			line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);

			if (line.empty())
			{
				continue;
			}

			m_stdout.emplace_back(line);
		}
	}
}