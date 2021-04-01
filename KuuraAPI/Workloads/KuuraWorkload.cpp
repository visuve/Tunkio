#include "../KuuraAPI-PCH.hpp"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	IWorkload::IWorkload(const CallbackContainer& callbacks, const std::filesystem::path& path, bool remove) :
		m_callbacks(callbacks),
		m_path(path),
		m_removeAfterWipe(remove)
	{
	}

	IWorkload::~IWorkload()
	{
	}

	uint16_t IWorkload::FillerCount() const
	{
		return static_cast<uint16_t>(m_fillers.size());
	}

	bool IWorkload::HasFillers() const
	{
		return !m_fillers.empty();
	}

	std::shared_ptr<IFillProvider> IWorkload::TakeFiller()
	{
		assert(HasFillers());
		const auto next = m_fillers.front();
		m_fillers.pop();
		return next;
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
				m_callbacks.OnError(path.c_str(), KuuraStage::Write, pass, bytesWritten, LastError);
				return false;
			}

			if (filler->Verify)
			{
				const auto actualData = fillable->Read(size, offset);

				if (!actualData.first)
				{
					m_callbacks.OnError(path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}

				if (!std::equal(writtenData.begin(), writtenData.end(), actualData.second.begin()))
				{
					m_callbacks.OnError(path.c_str(), KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}
			}

			if (!m_callbacks.OnProgress(path.c_str(), pass, bytesWritten))
			{
				return true;
			}
		}

		return true;
	}
}
