#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileWorkload.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileWorkload::FileWorkload(
		const CallbackContainer* callbacks,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(callbacks, path, removeAfterOverwrite)
	{
	}

	bool FileWorkload::Run(const std::vector<std::shared_ptr<IFillProvider>>& fillers)
	{
		auto file = std::make_shared<File>(_path);

		if (!file->IsValid())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file->Size())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		_callbacks->OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), file->Size().value());

		for (auto filler : fillers)
		{
			_callbacks->OnPassStarted(_path.c_str(), ++passes);

			uint64_t bytesLeft = file->Size().value();
			uint64_t bytesWritten = 0;

			if (!Overwrite(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file->Flush())
			{
				_callbacks->OnError(_path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			_callbacks->OnPassCompleted(_path.c_str(), passes);
		}

		_callbacks->OnOverwriteCompleted(passes, totalBytesWritten);

		if (_removeAfterOverwrite && !file->Delete())
		{
			_callbacks->OnError(_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
