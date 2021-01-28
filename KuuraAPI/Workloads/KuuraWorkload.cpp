#include "../KuuraAPI-PCH.hpp"
#include "KuuraWorkload.hpp"

namespace Kuura
{
	IWorkload::IWorkload(const std::filesystem::path& path, bool remove, void* context) :
		m_path(path),
		m_removeAfterWipe(remove),
		m_context(context)
	{
	}

	IWorkload::~IWorkload()
	{
	}

	void IWorkload::SetWipeStartedCallback(KuuraWipeStartedCallback* callback)
	{
		m_wipeStartedCallback = callback;
	}

	void IWorkload::SetPassStartedCallback(KuuraPassStartedCallback* callback)
	{
		m_passStartedCallback = callback;
	}

	void IWorkload::SetProgressCallback(KuuraProgressCallback* callback)
	{
		m_progressCallback = callback;
	}

	void IWorkload::SetPassCompletedCallback(KuuraPassCompletedCallback* callback)
	{
		m_passCompletedCallback = callback;
	}

	void IWorkload::SetWipeCompletedCallback(KuuraWipeCompletedCallback* callback)
	{
		m_wipeCompletedCallback = callback;
	}

	void IWorkload::SetErrorCallback(KuuraErrorCallback* callback)
	{
		m_errorCallback = callback;
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

	void IWorkload::OnWipeStarted(uint16_t passes, uint64_t bytesToWritePerPass)
	{
		if (!m_wipeStartedCallback)
		{
			return;
		}

		m_wipeStartedCallback(m_context, passes, bytesToWritePerPass);
	}

	void IWorkload::OnPassStarted(uint16_t pass)
	{
		if (!m_passStartedCallback)
		{
			return;
		}

		m_passStartedCallback(m_context, pass);
	}

	bool IWorkload::OnProgress(uint16_t pass, uint64_t bytesWritten)
	{
		if (!m_progressCallback)
		{
			return true;
		}

		return m_progressCallback(m_context, pass, bytesWritten);
	}

	void IWorkload::OnPassCompleted(uint16_t pass)
	{
		if (!m_passCompletedCallback)
		{
			return;
		}

		m_passCompletedCallback(m_context, pass);
	}

	void IWorkload::OnWipeCompleted(uint16_t passes, uint64_t totalBytesWritten)
	{
		if (!m_wipeCompletedCallback)
		{
			return;
		}

		m_wipeCompletedCallback(m_context, passes, totalBytesWritten);
	}

	void IWorkload::OnError(
		KuuraStage stage,
		uint16_t pass,
		uint64_t bytesWritten,
		uint32_t errorCode)
	{
		if (!m_errorCallback)
		{
			return;
		}

		m_errorCallback(m_context, stage, pass, bytesWritten, errorCode);
	}

	bool IWorkload::Fill(
		uint16_t pass,
		uint64_t bytesLeft,
		uint64_t& bytesWritten,
		std::shared_ptr<IFillProvider> filler,
		std::shared_ptr<IFillConsumer> fillable)
	{
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
				OnError(KuuraStage::Write, pass, bytesWritten, LastError);
				return false;
			}

			if (filler->Verify)
			{
				const auto actualData = fillable->Read(size, offset);

				if (!actualData.first)
				{
					OnError(KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}

				if (!std::equal(writtenData.begin(), writtenData.end(), actualData.second.begin()))
				{
					OnError(KuuraStage::Verify, pass, bytesWritten, LastError);
					return false;
				}
			}

			if (!OnProgress(pass, bytesWritten))
			{
				return true;
			}
		}

		return true;
	}
}
