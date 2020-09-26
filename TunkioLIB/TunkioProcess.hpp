#pragma once

namespace Tunkio
{
	template <typename T>
	class IProcess
	{
	public:
		IProcess(const std::filesystem::path& executable, const std::basic_string<T>& arguments) :
			m_executable(executable),
			m_arguments(arguments)
		{
		}

		std::filesystem::path Executable() const
		{
			return m_executable;
		}

		std::basic_string<T> Arguments() const
		{
			return m_arguments;
		}

		virtual bool Start() = 0;

		uint32_t ErrorCode() const
		{
			return m_errorCode;
		}

		uint32_t ExitCode() const
		{
			return m_exitCode;
		}

		const std::vector<std::string>& StdOut() const
		{
			return m_stdout;
		}

	protected:
		const std::filesystem::path m_executable;
		const std::basic_string<T> m_arguments;
		uint32_t m_errorCode = 0;
		uint32_t m_exitCode = 0;
		std::vector<std::string> m_stdout;
	};
}