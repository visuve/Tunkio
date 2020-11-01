#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioTime.hpp"
#include "TunkioDataUnits.hpp"
#include "TunkioMemory.hpp"

#if defined(_WIN32)
#include "Win32/TunkioWin32ChildProcess.hpp"
#else
#include "Posix/TunkioPosixChildProcess.hpp"
#endif

namespace Tunkio
{
	Time::Timer g_totalTimer;
	Time::Timer g_currentTimer;
	uint32_t g_error = ErrorCode::Success;
	uint64_t g_bytesToWrite = 0;
	uint64_t g_bytesWrittenLastTime = 0;
	std::atomic<bool> g_keepRunning = true;

	void SignalHandler(int signal)
	{
		std::cout << "Got signal: " << signal << std::endl;
		g_keepRunning = false;
	}

	void PrintUsage(const std::filesystem::path& exe)
	{
		std::cout << " Usage:" << std::endl << std::endl;
#if defined(_WIN32)
		std::cout << "  --path=\"P:\\Path\\To\\File or Drive\" (Required) " << std::endl;
#else
		std::cout << "  --path=/path/to/file_or_device (Required)" << std::endl;
#endif
		std::cout << "  --target=[f|d|D] where f=file, d=directory, D=drive (Optional) " << std::endl;
		std::cout << "  --mode=[0|1|r|c|s] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random, c=character, s=string. Note: with option 'c' or 's' you need to provide filler argument. (Optional) " << std::endl;
		std::cout << "  --remove=[y|n] remove on exit y=yes, n=no. Applies only to file or directory (Optional)" << std::endl;
		std::cout << "  --filler=[character|string] required when --mode=c or --mode=s" << std::endl;
		std::cout << std::endl;
		std::cout << " Usage examples:" << std::endl << std::endl;
#if defined(_WIN32)
		std::cout << "  " << exe.string() << " --path=\"C:\\SecretFile.txt\" --target=f --mode=r" << std::endl;
		std::cout << "  " << exe.string() << " --path=\"C:\\SecretDirectory\" --targetd --mode=r"<< std::endl;
		std::cout << "  " << exe.string() << " --path=\\\\.\\PHYSICALDRIVE9 --target=D --mode=r" << std::endl;
#else
		std::cout << "  " << exe.string() << " --path=/home/you/secret_file.txt --target=f --mode=r" << std::endl;
		std::cout << "  " << exe.string() << " --path=/home/you/secret_directory --target=d --mode=r" << std::endl;
		std::cout << "  " << exe.string() << " --path=/dev/sdx --target= --target=D --mode=r" << std::endl;
#endif
		std::cout << std::endl;

#if defined(_WIN32)
		Win32ChildProcess process(L"C:\\Windows\\System32\\wbem\\WMIC.exe", L"diskdrive list brief");
#else
		PosixChildProcess process("/bin/df", "-h");
#endif

		if (!process.Start())
		{
			std::cerr << "Failed to start " << process.Executable() << " to list disk drives. Error code: " << process.ErrorCode() << std::endl;
			return;
		}

		if (process.ExitCode() != 0)
		{
			std::cerr << "Listing your drives failed with " << process.Executable() << ". Exit code: " << process.ExitCode() << std::endl;
			return;
		}

		std::cout << std::endl << " Here are your drives:" << std::endl << std::endl;

		for (const std::string& line : process.StdOut())
		{
			std::cout << ' ' << line << std::endl;
		}
	}

	bool PrintArgumentsAndPrompt(const std::vector<std::string>& args)
	{
		const std::string joined = 
			std::accumulate(args.cbegin(), args.cend(), std::string(), [](const std::string& accumulated, const std::string& value)
		{
			return value.empty() ? accumulated : accumulated + "\n  " + value;
		});

		std::cout << "Provided arguments: " << joined << std::endl << "Are you sure you want to continue? [y/n]" << std::endl;
		const int prompt = getchar();
		std::cout << std::endl;
		return prompt == 'y' || prompt == 'Y';
	}

	std::map<std::string, Args::Argument> Arguments =
	{
		{ "path", Args::Argument(true, std::string()) },
		{ "target", Args::Argument(false, TunkioTargetType::File) },
		{ "mode", Args::Argument(false, TunkioFillType::Zeroes) },
		{ "remove", Args::Argument(false, false) },
		{ "filler", Args::Argument(true, std::string()) },
	};

	void OnStarted(uint16_t, uint64_t bytesLeft)
	{
		g_totalTimer.Reset();
		g_currentTimer.Reset();
		g_error = ErrorCode::Success;
		g_bytesToWrite = bytesLeft;
		g_bytesWrittenLastTime = 0;

		std::cout << Time::Timestamp() << " Started!" << std::endl;
	}

	bool OnProgress(uint16_t, uint64_t bytesWritten)
	{
		if (!g_keepRunning)
		{
			return false;
		}

		// TODO: current speed sometimes shows incorrectly
		const auto elapsedSince = g_currentTimer.Elapsed<Time::MilliSeconds>();
		const auto elapsedTotal = g_totalTimer.Elapsed<Time::MilliSeconds>();
		const DataUnit::Byte bytesWrittenSince(bytesWritten - g_bytesWrittenLastTime);
		const DataUnit::Byte bytesWrittenTotal(bytesWritten);

		if (bytesWrittenTotal.Value() && elapsedSince.count())
		{
			const DataUnit::Byte bytesLeft = g_bytesToWrite - bytesWritten;
			std::cout << DataUnit::HumanReadable(bytesWrittenTotal) << " written."
				<< " Speed: " << DataUnit::SpeedPerSecond(bytesWrittenSince, elapsedSince)
				<< ". Avg. speed: " << DataUnit::SpeedPerSecond(bytesWrittenTotal, elapsedTotal)
				<< ". Time left: " << Time::HumanReadable(DataUnit::TimeLeft(bytesLeft, bytesWrittenTotal, g_totalTimer))
				<< std::endl;
		}

		g_currentTimer.Reset();
		g_bytesWrittenLastTime = bytesWritten;
		return g_keepRunning;
	}

	void OnError(TunkioStage stage, uint16_t, uint64_t bytesWritten, uint32_t error)
	{
		std::cerr << Time::Timestamp() << " Error " << error << " occurred while ";
				
		switch (stage)
		{
			case TunkioStage::Open:
				std::cerr << "opening!";
				break;
			case TunkioStage::Size:
				std::cerr << "querying size!";
				break;
			case TunkioStage::Write:
				std::cerr << "writing!";
				break;
			case TunkioStage::Verify:
				std::cerr << "verifying!";
				break;
			case TunkioStage::Remove:
				std::cerr << "removing file!";
				break;
		}
			
		if (bytesWritten)
		{
			std::cerr << " Bytes written: " << bytesWritten;
		}

		std::cerr << std::endl;

#ifdef WIN32
		std::array<wchar_t, 0x400> buffer;
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
			std::wcerr << L"Detailed description: " << std::wstring(buffer.data(), size - 2) << std::endl; // Trim excess /r/n
		}
#endif

		g_error = error;
	}

	void OnCompleted(uint16_t, uint64_t bytesWritten)
	{
		std::cout << Time::Timestamp() << " Finished. Bytes written: " << bytesWritten << std::endl;

		const DataUnit::Byte bytes(bytesWritten);
		const auto elapsed = g_totalTimer.Elapsed<Time::MilliSeconds>();

		if (bytes.Value() && elapsed.count())
		{
			std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
		}
	}

	bool CheckArguments()
	{
		auto mode = Arguments.at("mode").Value<TunkioFillType>();
		auto filler = Arguments.at("filler").Value<std::string>();

		if (mode == TunkioFillType::Character)
		{
			if (filler.size() != 1)
			{
				std::cerr << "Filler argument needs to be one character!" << std::endl << std::endl;
				return false;
			}
		}
		else if (mode == TunkioFillType::String)
		{
			if (filler.empty())
			{
				std::cerr << "String argument needts to be at least one character!" << std::endl << std::endl;
				return false;
			}
		}
		else if (!filler.empty())
		{
			std::cerr << "Filler argument can only be used with --mode=c or --mode=s!" << std::endl << std::endl;
			return false;
		}

		return true;
	}
}

int main(int argc, char* argv[])
{
	using namespace Tunkio;

	std::cout << std::endl;
	std::cout << "#######   #     #   #     #   #   #   ###    #####" << std::endl;
	std::cout << "   #      #     #   # #   #   #  #     #    #     #" << std::endl;
	std::cout << "   #      #     #   #  #  #   ###      #    #     #" << std::endl;
	std::cout << "   #      #     #   #   # #   #  #     #    #     #" << std::endl;
	std::cout << "   #       #####    #     #   #   #   ###    #####" << std::endl;
	std::cout << std::endl << "Version 0.1" << std::endl << std::endl;

	if (argc <= 1)
	{
		std::cerr << "Too few arguments!" << std::endl << std::endl;
		PrintUsage(argv[0]);
		return ErrorCode::InvalidArgument;
	}

	const std::vector<std::string> args({ argv + 1, argv + argc });

	if (std::signal(SIGINT, Tunkio::SignalHandler) == SIG_ERR)
	{
		std::cerr << "Cannot attach CTRL+C interrupt handler!" << std::endl;
		return ErrorCode::Generic;
	}

	if (!Args::ParseVector(Arguments, args))
	{
		std::cerr << "Invalid arguments!" << std::endl << std::endl;
		PrintUsage(argv[0]);
		return ErrorCode::InvalidArgument;
	}

	if (!CheckArguments())
	{
		PrintUsage(argv[0]);
		return ErrorCode::InvalidArgument;
	}

	if (!PrintArgumentsAndPrompt(args))
	{
		return ErrorCode::UserCancelled;
	}

	const Tunkio::Memory::AutoHandle tunkio(TunkioInitialize(
		Arguments.at("path").Value<std::string>().c_str(),
		Arguments.at("target").Value<TunkioTargetType>()));

	if (!tunkio)
	{
		std::cerr << "Failed to create TunkioHandle!" << std::endl;
		return ErrorCode::InvalidArgument;
	}

	auto mode = Arguments.at("mode").Value<TunkioFillType>();
	auto filler = Arguments.at("filler").Value<std::string>();

	if (!TunkioAddWipeRound(tunkio.get(), mode, filler.c_str()) ||
		!TunkioSetStartedCallback(tunkio.get(), OnStarted) ||
		!TunkioSetProgressCallback(tunkio.get(), OnProgress) ||
		!TunkioSetCompletedCallback(tunkio.get(), OnCompleted) ||
		!TunkioSetErrorCallback(tunkio.get(), OnError) ||
		!TunkioSetRemoveAfterFill(tunkio.get(), Arguments.at("remove").Value<bool>()))
	{
		std::cerr << "Failed to pass arguments!" << std::endl;
		return ErrorCode::InvalidArgument;
	}

	if (!TunkioRun(tunkio.get()))
	{
		std::cerr << "Tunkio failed." << std::endl;
	}

	return g_error;
}