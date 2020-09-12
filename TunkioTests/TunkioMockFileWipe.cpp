#include "PCH.hpp"
#include "TunkioFileWipe.hpp"
#include "TunkioFillStrategy.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool FileWipe::Run()
	{
		m_options->Callbacks.StartedCallback(1);
		m_options->Callbacks.ProgressCallback(1);
		m_options->Callbacks.CompletedCallback(1);
		return true;
	}
}