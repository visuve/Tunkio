#include "PCH.hpp"
#include "TunkioFileWipe.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool FileWipe::Run()
	{
		m_startedCallback(1);
		m_progressCallback(1);
		m_completedCallback(1);
		return true;
	}
}