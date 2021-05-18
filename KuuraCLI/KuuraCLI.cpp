#include "KuuraCLI-PCH.hpp"
#include "KuuraCLI.hpp"

namespace Kuura
{
	CLI::~CLI()
	{
		_keepRunning = false;

		if (_handle)
		{
			KuuraFree(_handle);
		}
	}

	bool CLI::Initialize(
		KuuraTargetType targetType,
		const std::filesystem::path& targetPath,
		KuuraFillType fillType,
		const std::string& fillValue,
		bool verify,
		bool remove)
	{
		_handle = KuuraInitialize(this);

		if (!KuuraAddTarget(_handle, targetType, targetPath.c_str(), remove))
		{
			std::cerr << "KuuraAddTarget failed!" << std::endl;
			return false;
		}

		if (!KuuraAddPass(_handle, fillType, verify, fillValue.c_str()))
		{
			std::cerr << "KuuraAddOverwriteRound failed!" << std::endl;
			return false;
		}

		KuuraSetOverwriteStartedCallback(_handle, [](
			void* context,
			uint16_t passes,
			uint64_t bytesToWritePerPass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnOverwriteStarted(passes, bytesToWritePerPass);
		});

		KuuraSetTargetStartedCallback(_handle, [](
			void* context,
			const KuuraChar* path,
			uint64_t bytesToWritePerPass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnTargetStarted(path, bytesToWritePerPass);
		});

		KuuraSetPassStartedCallback(_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassStarted(path, pass);
		});

		KuuraSetProgressCallback(_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass,
			uint64_t bytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			return self->OnProgress(path, pass, bytesWritten);
		});

		KuuraSetPassCompletedCallback(_handle, [](
			void* context,
			const KuuraChar* path,
			uint16_t pass)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnPassCompleted(path, pass);
		});

		KuuraSetTargetCompletedCallback(_handle, [](
			void* context,
			const KuuraChar* path,
			uint64_t bytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnTargetCompleted(path, bytesWritten);
		});

		KuuraSetOverwriteCompletedCallback(_handle, [](
			void* context,
			uint16_t passes,
			uint64_t totalBytesWritten)
		{
			auto self = reinterpret_cast<CLI*>(context);
			self->OnOverwriteCompleted(passes, totalBytesWritten);
		});

		KuuraSetErrorCallback(_handle, [](
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
		return KuuraRun(_handle);
	}

	void CLI::Stop()
	{
		_keepRunning = false;
	}

	int32_t CLI::Error() const
	{
		return _error;
	}

	void CLI::OnOverwriteStarted(uint16_t passes, uint64_t bytesLeft)
	{
		_bytesToWrite = bytesLeft;
		_bytesWrittenLastTime = 0;

		std::cout << Time::Timestamp() << " Overwrite Started! Passes " << passes << '.' << std::endl;
	}

	void CLI::OnTargetStarted(const std::filesystem::path& path, uint64_t)
	{
		std::cout << Time::Timestamp() << " Starting to overwrite: " << path << '.' << std::endl;
	}

	void CLI::OnPassStarted(const std::filesystem::path&, uint16_t pass)
	{
		_totalTimer.Reset();
		_currentTimer.Reset();

		std::cout << Time::Timestamp() << " Pass " << pass << " started!" << std::endl;
	}

	bool CLI::OnProgress(const std::filesystem::path&, uint16_t, uint64_t bytesWritten)
	{
		if (!_keepRunning)
		{
			return false;
		}

		// TODO: current speed sometimes shows incorrectly
		const auto elapsedSince = _currentTimer.Elapsed<Time::MilliSeconds>();
		const auto elapsedTotal = _totalTimer.Elapsed<Time::MilliSeconds>();
		const DataUnit::Bytes bytesWrittenSince(bytesWritten - _bytesWrittenLastTime);
		const DataUnit::Bytes bytesWrittenTotal(bytesWritten);

		if (bytesWrittenTotal.Bytes() && elapsedSince.count())
		{
			const DataUnit::Bytes bytesLeft = _bytesToWrite - bytesWritten;
			std::cout << DataUnit::HumanReadable(bytesWrittenTotal) << " written."
				<< " Speed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince)
				<< ". Avg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal)
				<< ". Time left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, _totalTimer))
				<< ". " << std::setprecision(3) << std::fixed << (float(bytesWritten) / float(_bytesToWrite)) * 100.0f
				<< "% complete.\n";
		}

		_currentTimer.Reset();
		_bytesWrittenLastTime = bytesWritten;
		return _keepRunning;
	}

	void CLI::OnPassCompleted(const std::filesystem::path&, uint16_t pass)
	{
		std::cout << Time::Timestamp() << " Pass " << pass << " completed!" << std::endl;
	}

	void CLI::OnTargetCompleted(const std::filesystem::path& path, uint64_t)
	{
		std::cout << Time::Timestamp() << ' ' << path << " processed." << std::endl;
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
		_error = error;
	}

	void CLI::OnOverwriteCompleted(uint16_t passes, uint64_t bytesWritten)
	{
		std::cout << Time::Timestamp() << " Overwrite complete. " <<
			"Passes "  << passes <<  ". Bytes written: " << bytesWritten << std::endl;

		const DataUnit::Bytes bytes(bytesWritten);
		const auto elapsed = _totalTimer.Elapsed<Time::MilliSeconds>();

		if (bytes.Value() && elapsed.count())
		{
			std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
		}
	}

}
