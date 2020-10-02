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
		const std::string command = m_executable.string() + " " + m_arguments;

		FILE* pipe = popen(command.c_str(), "r");

		if (!pipe)
		{
			m_errorCode = errno;
			return false;
		}

		std::array<char, 0xFF> buffer;
		std::stringstream stream;

		while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
		{
			stream << buffer.data();
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

		m_exitCode = WEXITSTATUS(pclose(pipe));
		return true;
	}
}