#include "PCH.hpp"
#include "TunkioDirectoryWipe.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool DirectoryWipe::Run()
	{
		m_options->Callbacks.StartedCallback(1);
		m_options->Callbacks.ProgressCallback(1);
		m_options->Callbacks.CompletedCallback(1);
		return true;
	}
}