#include "../KuuraAPI-PCH.hpp"
#include "KuuraDirectoryOverwrite.hpp"
#include "../FillConsumers/KuuraDirectory.hpp"
#include "../FillProviders/KuuraFillProvider.hpp"

namespace Kuura
{
	DirectoryOverwrite::DirectoryOverwrite(
		const Composer* parent,
		const std::filesystem::path& path,
		bool removeAfterOverwrite) :
		IWorkload(parent, path, removeAfterOverwrite)
	{
	}

	bool DirectoryOverwrite::Run()
	{
		Directory directory(_path);

		std::optional<std::vector<std::shared_ptr<File>>>& files = directory.Files();

		if (!files)
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Open, 0, 0, LastError);
			return false;
		}

		std::optional<uint64_t> directorySize = directory.Size();

		if (!directorySize)
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Size, 0, 0, LastError);
			return false;
		}

		uint16_t passes = 0;
		uint64_t totalBytesWritten = 0;

		auto fillers  = _parent->Fillers();
		_parent->Callbacks.OnOverwriteStarted(static_cast<uint16_t>(fillers.size()), directorySize.value());

		for (auto& filler : fillers)
		{
			_parent->Callbacks.OnPassStarted(_path.c_str(), ++passes);

			for (auto& file : files.value())
			{
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
			}

			_parent->Callbacks.OnPassCompleted(_path.c_str(), passes);
		}

		if (_removeAfterOverwrite && !directory.RemoveAll())
		{
			_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Delete, passes, totalBytesWritten, LastError);
			return false;
		}

		_parent->Callbacks.OnOverwriteCompleted(passes, totalBytesWritten);
		return true;
	}
}
