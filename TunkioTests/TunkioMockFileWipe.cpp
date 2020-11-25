#include "TunkioTests-PCH.hpp"
#include "Workloads/TunkioFileWipe.hpp"

namespace Tunkio
{
	FileWipe::FileWipe(
		const std::filesystem::path& path,
		bool removeAfterWipe,
		void* context) :
		IWorkload(path, removeAfterWipe, context)
	{
	}

	bool FileWipe::Run()
	{
		OnWipeStarted(1, 1);

		while (HasFillers())
		{
			TakeFiller();
			OnPassStarted(1);
			OnProgress(1, 1);
			OnPassCompleted(1);
		}

		OnWipeCompleted(1, 1);
		return true;
	}
}
