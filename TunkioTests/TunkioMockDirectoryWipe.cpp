#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioDirectoryWipe.hpp"

namespace Tunkio
{
	DirectoryWipe::DirectoryWipe(void* context, const std::string& path) :
		IWorkload(context, path)
	{
	}

	bool DirectoryWipe::Run()
	{
		OnStarted(1, 1);
		OnProgress(1, 1);
		OnCompleted(1, 1);
		return true;
	}
}
