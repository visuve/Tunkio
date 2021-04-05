#include "../KuuraAPI-PCH.hpp"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	IWorkload::IWorkload(const Composer* parent, const std::filesystem::path& path, bool remove) :
		_parent(parent),
		_path(path),
		_removeAfterOverwrite(remove)
	{
	}

	IWorkload::~IWorkload()
	{
	}

	bool IWorkload::Overwrite(
		uint16_t pass,
		uint64_t bytesLeft,
		uint64_t& bytesWritten,
		std::shared_ptr<IFillProvider> provider,
		std::shared_ptr<IFillConsumer> consumer)
	{
		while (bytesLeft)
		{
			const uint64_t size = std::min(bytesLeft, consumer->OptimalWriteSize().value());
			const uint64_t offset = bytesWritten;

			const std::span<std::byte> writtenData = provider->Data(size, consumer->AlignmentSize().value());
			const auto result = consumer->Write(writtenData);

			bytesWritten += result.second;
			bytesLeft -= std::min(bytesLeft, result.second);

			if (!result.first)
			{
				_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Write, pass, bytesWritten, LastError);
				return false;
			}

			if (provider->Verify)
			{
				const auto actualData = consumer->Read(size, offset);

				if (!actualData.first)
				{
					_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}

				if (!std::equal(writtenData.begin(), writtenData.end(), actualData.second.begin()))
				{
					_parent->Callbacks.OnError(_path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}
			}

			if (!_parent->Callbacks.OnProgress(_path.c_str(), pass, bytesWritten))
			{
				return true;
			}
		}

		return true;
	}
}
