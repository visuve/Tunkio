#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioFileWipe.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(void* context, const std::string& path) :
		IWorkload(context, path)
	{
	}

	bool FileWipe::Run()
	{
		OnStarted(1, 1);
		OnProgress(1, 1);
		OnCompleted(1, 1);
		return true;
	}
}
