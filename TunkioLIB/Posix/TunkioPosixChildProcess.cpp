#include "../PCH.hpp"
#include "TunkioPosixChildProcess.hpp"

namespace Tunkio
{
	PosixChildProcess::PosixChildProcess(const std::filesystem::path& executable, const std::string& arguments) :
		IProcess(executable, arguments)
	{
	}

	bool PosixChildProcess::Start()
	{
		std::array<char, 0xFF> buffer;

		const std::string command = m_executable.string() + " " + m_arguments;

		FILE* pipe = popen(command.c_str(), "r");

		if (!pipe)
		{
			m_errorCode = errno;
			return false;
		}

		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			m_stdout += buffer.data();
		}

		m_exitCode = WEXITSTATUS(pclose(pipe));
		return true;
	}
}