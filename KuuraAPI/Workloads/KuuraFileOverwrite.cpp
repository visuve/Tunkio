#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileOverwrite.hpp"
#include "../FillConsumers/KuuraFile.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	FileOverwrite::FileOverwrite(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(parent, path, removeAfterOverwrite)
	{
	}

	bool FileOverwrite::Run()
	{
		auto file = std::make_shared<File>(_path);

		if (!file->IsValid())
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		if (!file->Size())
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers = _parent->Fillers();

		_parent->Callbacks.OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), file->Size().value());

		for (auto filler : fillers)
		{
			_parent->Callbacks.OnPassStarted(_path.c_str(), ++passes);

			uint64_t bytesLeft = file->Size().value();
			uint64_t bytesWritten = 0;

			if (!Overwrite(passes, bytesLeft, bytesWritten, filler, file))
			{
				return false;
			}

			if (!file->Flush())
			{
				_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Write, passes, bytesWritten, LastError);
			}

			totalBytesWritten += bytesWritten;
			_parent->Callbacks.OnPassCompleted(_path.c_str(), passes);
		}

		_parent->Callbacks.OnOverwriteCompleted(passes, totalBytesWritten);

		if (_removeAfterOverwrite && !file->Delete())
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		return true;
	}
}
