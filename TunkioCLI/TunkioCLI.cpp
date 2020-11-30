#include "TunkioCLI-PCH.hpp"
#include "TunkioCLI.hpp"

namespace Tunkio
{
	CLI::~CLI()
	{
		m_keepRunning = false;

		if (m_handle)
		{
			TunkioFree(m_handle);
		}
	}

	bool CLI::Initialize(
		const std::filesystem::path& path,
		TunkioTargetType targetType,
		TunkioFillType fillType,
		const std::string& filler,
		bool verify,
		bool remove)
	{
		m_handle = TunkioInitialize(path.string().c_str(), targetType, remove, this);

		if (!m_handle)
		{
			std::cerr << "TunkioInitialize failed!" << std::endl;
			return false;
		}

		TunkioSetWipeStartedCallback(m_handle, [](
			void* context,
			uint16_t passes,
			uint64_t bytesToWritePerPass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnWipeStarted(passes, bytesToWritePerPass);
		});

		TunkioSetPassStartedCallback(m_handle, [](
			void* context,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassStarted(pass);
		});

		TunkioSetProgressCallback(m_handle, [](
			void* context,
			uint16_t pass,
			uint64_t bytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			return self->OnProgress(pass, bytesWritten);
		});

		TunkioSetPassCompletedCallback(m_handle, [](
			void* context,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassCompleted(pass);
		});

		TunkioSetWipeCompletedCallback(m_handle, [](
			void* context,
			uint16_t passes,
			uint64_t totalBytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnWipeCompleted(passes, totalBytesWritten);
		});

		TunkioSetErrorCallback(m_handle, [](
			void* context,
			TunkioStage stage,
			uint16_t pass,
			uint64_t bytesWritten,
			uint32_t error)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnError(stage, pass, bytesWritten, error);
		});

		return TunkioAddWipeRound(m_handle, fillType, verify, filler.c_str());
	}

	bool CLI::Run()
	{
		return TunkioRun(m_handle);
	}

	void CLI::Stop()
	{
		m_keepRunning = false;
	}

	int32_t CLI::Error() const
	{
		return m_error;
	}

	void CLI::OnWipeStarted(uint16_t passes, uint64_t bytesLeft)
	{
		m_bytesToWrite = bytesLeft;
		m_bytesWrittenLastTime = 0;

		std::cout << Time::Timestamp() << " Wipe Started! Passes " << passes << '.' << std::endl;
	}

	void CLI::OnPassStarted(uint16_t pass)
	{
		m_totalTimer.Reset();
		m_currentTimer.Reset();

		std::cout << Time::Timestamp() << " Pass " << pass << " started!" << std::endl;
	}

	bool CLI::OnProgress(uint16_t, uint64_t bytesWritten)
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

	void CLI::OnPassCompleted(uint16_t pass)
	{
		std::cout << Time::Timestamp() << " Pass " << pass << " completed!" << std::endl;
	}

	void CLI::OnError(TunkioStage stage, uint16_t, uint64_t bytesWritten, uint32_t error)
	{
		std::cerr << Time::Timestamp() << " Error " << error << " occurred while ";

		switch (stage)
		{
			case TunkioStage::Open:
				std::cerr << "opening";
				break;
			case TunkioStage::Unmount:
				std::cerr << "unmounting";
				break;
			case TunkioStage::Size:
				std::cerr << "querying size";
				break;
			case TunkioStage::Write:
				std::cerr << "writing";
				break;
			case TunkioStage::Rewind:
				std::cerr << "rewinding file pointer";
				break;
			case TunkioStage::Verify:
				std::cerr << "verifying";
				break;
			case TunkioStage::Delete:
				std::cerr << "deleting file";
				break;
		}

		std::cerr << "! Bytes written: " << bytesWritten << '.' << std::endl;

#if defined(_WIN32)
		std::array<wchar_t, 0x400> buffer = {};
		DWORD size = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			nullptr);

		if (size > 2)
		{
			std::wcerr << L"Detailed description: ";
			std::wcerr.write(buffer.data(), size - 2) << std::endl; // Trim excess /r/n
		}
#else
		std::cerr << "Detailed description: ";
		std::cerr << strerror(error) << '.' << std::endl;
#endif
		m_error = error;
	}

	void CLI::OnWipeCompleted(uint16_t passes, uint64_t bytesWritten)
	{
		std::cout << Time::Timestamp() << " Wipe complete. " <<
			"Passes "  << passes <<  ". Bytes written: " << bytesWritten << std::endl;

		const DataUnit::Bytes bytes(bytesWritten);
		const auto elapsed = m_totalTimer.Elapsed<Time::MilliSeconds>();

		if (bytes.Value() && elapsed.count())
		{
			std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
		}
	}

}
