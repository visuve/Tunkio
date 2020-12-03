#include "../TunkioAPI-PCH.hpp"
#include "TunkioWorkload.hpp"

namespace Tunkio
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

	void IWorkload::SetWipeStartedCallback(TunkioWipeStartedCallback* callback)
	{
		m_wipeStartedCallback = callback;
	}

	void IWorkload::SetPassStartedCallback(TunkioPassStartedCallback* callback)
	{
		m_passStartedCallback = callback;
	}

	void IWorkload::SetProgressCallback(TunkioProgressCallback* callback)
	{
		m_progressCallback = callback;
	}

	void IWorkload::SetPassCompletedCallback(TunkioPassCompletedCallback* callback)
	{
		m_passCompletedCallback = callback;
	}

	void IWorkload::SetWipeCompletedCallback(TunkioWipeCompletedCallback* callback)
	{
		m_wipeCompletedCallback = callback;
	}

	void IWorkload::SetErrorCallback(TunkioErrorCallback* callback)
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
		TunkioStage stage,
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

	bool IWorkload::Fill(uint16_t pass, uint64_t bytesLeft, uint64_t& bytesWritten, std::shared_ptr<IFillProvider> filler, File& file)
	{
		while (bytesLeft)
		{
			const uint64_t size = std::min(bytesLeft, file.OptimalWriteSize().second);
			const uint64_t offset = bytesWritten;

			const std::span<std::byte> writtenData = filler->Data(size, file.AlignmentSize().second);
			const auto result = file.Write(writtenData);

			bytesWritten += result.second;
			bytesLeft -= result.second;

			if (!result.first)
			{
				OnError(TunkioStage::Write, pass, bytesWritten, LastError);
				return false;
			}

			if (filler->Verify)
			{
				const auto actualData = file.Read(size, offset);

				if (!actualData.first)
				{
					OnError(TunkioStage::Verify, pass, bytesWritten, LastError);
					return false;
				}

				if (!std::equal(writtenData.begin(), writtenData.end(), actualData.second.begin()))
				{
					OnError(TunkioStage::Verify, pass, bytesWritten, LastError);
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
