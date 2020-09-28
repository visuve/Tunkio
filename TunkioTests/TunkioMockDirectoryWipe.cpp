#include "PCH.hpp"
#include "TunkioDirectoryWipe.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool DirectoryWipe::Run()
	{
		m_startedCallback(1);
		m_progressCallback(1);
		m_completedCallback(1);
		return true;
	}
}