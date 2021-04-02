#include "KuuraCLI-PCH.hpp"
#include "KuuraCLI.hpp"

namespace Kuura
{
	CLI::~CLI()
	{
		m_keepRunning = false;

		if (m_handle)
		{
			KuuraFree(m_handle);
		}
	}

	bool CLI::Initialize(
		const std::filesystem::path& path,
		KuuraTargetType targetType,
		KuuraFillType fillType,
		const std::string& filler,
		bool verify,
		bool remove)
	{
		m_handle = KuuraInitialize(this);

		if (!KuuraAddTarget(m_handle, path.c_str(), targetType, remove))
		{
			std::cerr << "KuuraAddTarget failed!" << std::endl;
			return false;
		}

		if (!KuuraAddOverwriteRound(m_handle, fillType, verify, filler.c_str()))
		{
			std::cerr << "KuuraAddOverwriteRound failed!" << std::endl;
			return false;
		}

		KuuraSetOverwriteStartedCallback(m_handle, [](
			void* context,
			uint16_t passes,
			uint64_t bytesToWritePerPass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnOverwriteStarted(passes, bytesToWritePerPass);
		});

		KuuraSetPassStartedCallback(m_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassStarted(path, pass);
		});

		KuuraSetProgressCallback(m_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass,
			uint64_t bytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			return self->OnProgress(path, pass, bytesWritten);
		});

		KuuraSetPassCompletedCallback(m_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassCompleted(path, pass);
		});

		KuuraSetOverwriteCompletedCallback(m_handle, [](
			void* context,
			uint16_t passes,
			uint64_t totalBytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnOverwriteCompleted(passes, totalBytesWritten);
		});

		KuuraSetErrorCallback(m_handle, [](
			void* context,
			const KuuraChar* path,
			KuuraStage stage,
			uint16_t pass,
			uint64_t bytesWritten,
			uint32_t error)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnError(path, stage, pass, bytesWritten, error);
		});

		return true;
	}

	bool CLI::Run()
	{
		return KuuraRun(m_handle);
	}

	void CLI::Stop()
	{
		m_keepRunning = false;
	}

	int32_t CLI::Error() const
	{
		return m_error;
	}

	void CLI::OnOverwriteStarted(uint16_t passes, uint64_t bytesLeft)
	{
		m_bytesToWrite = bytesLeft;
		m_bytesWrittenLastTime = 0;

		std::cout << Time::Timestamp() << " Overwrite Started! Passes " << passes << '.' << std::endl;
	}

	void CLI::OnPassStarted(const std::filesystem::path&, uint16_t pass)
	{
		m_totalTimer.Reset();
		m_currentTimer.Reset();

		std::cout << Time::Timestamp() << " Pass " << pass << " started!" << std::endl;
	}

	bool CLI::OnProgress(const std::filesystem::path&, uint16_t, uint64_t bytesWritten)
	{
		if (!m_keepRunning)
		{
			return false;
		}

		// TODO: current speed sometimes shows incorrectly
		const auto elapsedSince = m_currentTimer.Elapsed<Time::MilliSeconds>();
		const auto elapsedTotal = m_totalTimer.Elapsed<Time::MilliSeconds>();
		const DataUnit::Bytes bytesWrittenSince(bytesWritten - m_bytesWrittenLastTime);
		const DataUnit::Bytes bytesWrittenTotal(bytesWritten);

		if (bytesWrittenTotal.Bytes() && elapsedSince.count())
		{
			const DataUnit::Bytes bytesLeft = m_bytesToWrite - bytesWritten;
			std::cout << DataUnit::HumanReadable(bytesWrittenTotal) << " written."
				<< " Speed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince)
				<< ". Avg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal)
				<< ". Time left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, m_totalTimer))
				<< std::endl;
		}

		m_currentTimer.Reset();
		m_bytesWrittenLastTime = bytesWritten;
		return m_keepRunning;
	}

	void CLI::OnPassCompleted(const std::filesystem::path&, uint16_t pass)
	{
		std::cout << Time::Timestamp() << " Pass " << pass << " completed!" << std::endl;
	}

	void CLI::OnError(const std::filesystem::path& path, KuuraStage stage, uint16_t, uint64_t bytesWritten, uint32_t error)
	{
		std::cerr << Time::Timestamp() << " Error " << error << " occurred while ";

		switch (stage)
		{
			case KuuraStage::Open:
				std::cerr << "opening";
				break;
			case KuuraStage::Unmount:
				std::cerr << "unmounting";
				break;
			case KuuraStage::Size:
				std::cerr << "querying size";
				break;
			case KuuraStage::Write:
				std::cerr << "writing";
				break;
			case KuuraStage::Verify:
				std::cerr << "verifying";
				break;
			case KuuraStage::Delete:
				std::cerr << "deleting";
				break;
		}

		std::cerr << ' ' << path << "! Bytes written: " << bytesWritten << '.' << std::endl;
		std::cerr << "Detailed description: " << SystemErrorCodeToString(error) << std::endl;
		m_error = error;
	}

	void CLI::OnOverwriteCompleted(uint16_t passes, uint64_t bytesWritten)
	{
		std::cout << Time::Timestamp() << " Overwrite complete. " <<
			"Passes "  << passes <<  ". Bytes written: " << bytesWritten << std::endl;

		const DataUnit::Bytes bytes(bytesWritten);
		const auto elapsed = m_totalTimer.Elapsed<Time::MilliSeconds>();

		if (bytes.Value() && elapsed.count())
		{
			std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
		}
	}

}
