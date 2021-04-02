#include "../KuuraAPI-PCH.hpp"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	IWorkload::IWorkload(const Composer* parent, const std::filesystem::path& path, bool remove) :
		m_parent(parent),
		m_path(path),
		m_removeAfterWipe(remove)
	{
	}

	IWorkload::~IWorkload()
	{
	}

	bool IWorkload::Fill(
		uint16_t pass,
		uint64_t bytesLeft,
		uint64_t& bytesWritten,
		std::shared_ptr<IFillProvider> filler,
		std::shared_ptr<IFillConsumer> fillable)
	{
		const auto path = m_path.string();

		while (bytesLeft)
		{
			const uint64_t size = std::min(bytesLeft, fillable->OptimalWriteSize().value());
			const uint64_t offset = bytesWritten;

			const std::span<std::byte> writtenData = filler->Data(size, fillable->AlignmentSize().value());
			const auto result = fillable->Write(writtenData);

			bytesWritten += result.second;
			bytesLeft -= std::min(bytesLeft, result.second);

			if (!result.first)
			{
				m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Write, pass, bytesWritten, LastError);
				return false;
			}

			if (filler->Verify)
			{
				const auto actualData = fillable->Read(size, offset);

				if (!actualData.first)
				{
					m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}

				if (!std::equal(writtenData.begin(), writtenData.end(), actualData.second.begin()))
				{
					m_parent->Callbacks.OnError(path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}
			}

			if (!m_parent->Callbacks.OnProgress(path.c_str(), pass, bytesWritten))
			{
				return true;
			}
		}

		return true;
	}
}
