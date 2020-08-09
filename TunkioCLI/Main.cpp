#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioTime.hpp"
#include "TunkioDataUnits.hpp"
#include "TunkioMemory.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include "TunkioWin32ChildProcess.hpp"
#else
#include "TunkioPosixChildProcess.hpp"
#endif

namespace Tunkio
{
    Time::Timer g_totalTimer;
    Time::Timer g_currentTimer;
    uint32_t g_error = ErrorCode::Success;
    uint64_t g_bytesToWrite = 0;
    uint64_t g_bytesWrittenLastTime = 0;
    std::atomic<bool> g_keepRunning = true;

    void PrintUsage(const std::filesystem::path& exe)
    {
        std::cout << " Usage:" << std::endl << std::endl;
#if defined(_WIN32) || defined(_WIN64)
        std::cout << "  --path=\"P:\\Path\\To\\File or Device\" (Required) " << std::endl;
#else
        std::cout << "  --path=/path/to/file_or_device" (Required)" << std::endl;
#endif
        std::cout << "  --target=[f|d|m] where f=file, d=directory, m=mass storage device (Optional) " << std::endl;
        std::cout << "  --mode=[0|1|r] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random (Optional)" << std::endl;
        std::cout << "  --remove=[y|n] remove on exit y=yes, n=no. Applies only to file or directory (Optional)" << std::endl;
        std::cout << std::endl;
        std::cout << " Usage examples:" << std::endl << std::endl;
#if defined(_WIN32) || defined(_WIN64)
        std::cout << "  " << exe.string() << " --path=\"C:\\SecretFile.txt\" --target=" << char(TunkioTarget::File) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=\"C:\\SecretDirectory\" --target=" << char(TunkioTarget::Directory) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=\\\\.\\PHYSICALDRIVE9 --target=" << char(TunkioTarget::Device) << " --mode=r" << std::endl;
#else
        std::cout << "  " << exe.string() << " --path=/home/you/secret_file.txt --target=" << char(TunkioTarget::File) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=/home/you/secret_directory --target=" << char(TunkioTarget::Directory) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=/dev/sdx --target=" << char(TunkioTarget::Device) << " --mode=r" << std::endl;
#endif
        std::cout << std::endl;

#if defined(_WIN32) || defined(_WIN64)
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

        std::cout << std::endl << "Here are your drives:" << std::endl << std::endl;
        std::cout << process.StdOut() << std::endl;
    }

    bool PrintArgumentsAndPrompt(const std::vector<std::string>& args)
    {
        const std::string joined = std::accumulate(args.cbegin(), args.cend(), std::string(), [](const std::string& accumulated, const std::string& value)
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
        { "target", Args::Argument(false, TunkioTarget::File) },
        { "mode", Args::Argument(false, TunkioMode::Zeroes) },
        { "remove", Args::Argument(false, false) },
        { "path", Args::Argument(true, std::filesystem::path()) }
    };

    void OnStarted(uint64_t bytesLeft)
    {
        g_totalTimer.Reset();
        g_currentTimer.Reset();
        g_error = ErrorCode::Success;
        g_bytesToWrite = bytesLeft;
        g_bytesWrittenLastTime = 0;

        std::cout << Time::Timestamp() << " Started!" << std::endl;
    }

    bool OnProgress(uint64_t bytesWritten)
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

    void OnError(uint32_t error, uint64_t bytesWritten)
    {
        if (bytesWritten)
        {
            std::cerr << Time::Timestamp() << " Error " << error << " occurred. Bytes written: " << bytesWritten << std::endl;
        }
        else
        {
            std::cerr << Time::Timestamp() << " Error " << error << " occurred." << std::endl;
        }

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

    void OnCompleted(uint64_t bytesWritten)
    {
        std::cout << Time::Timestamp() << " Finished. Bytes written: " << bytesWritten << std::endl;

        const DataUnit::Byte bytes(bytesWritten);
        const auto elapsed = g_totalTimer.Elapsed<Time::MilliSeconds>();

        if (bytes.Value() && elapsed.count())
        {
            std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
        }
    }

    TunkioOptions* CreateOptions()
    {
        std::cout << std::setprecision(3) << std::fixed;

        const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

        return new TunkioOptions
        {
            Arguments.at("target").Value<TunkioTarget>(),
            Arguments.at("mode").Value<TunkioMode>(),
            Arguments.at("remove").Value<bool>(),
            TunkioCallbacks { OnStarted, OnProgress, OnError, OnCompleted },
            TunkioString{ static_cast<uint32_t>(path.size()), Memory::CloneString(path) }
        };
    }

    void SignalHandler(int signal)
    {
        std::cout << "Got signal: " << signal << std::endl;
        g_keepRunning = false;
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
        std::cerr << "Invalid arguments!" << std::endl << std::endl;
        PrintUsage(argv[0]);
        return ErrorCode::InvalidArgument;
    }

    const std::vector<std::string> args({ argv + 1, argv + argc });

    if (std::signal(SIGINT, Tunkio::SignalHandler) == SIG_ERR)
    {
        std::cerr << "Cannot attach SIGINT handler!" << std::endl;
        return EXIT_FAILURE;
    }

    if (!Args::ParseVector(Arguments, args))
    {
        std::cerr << "Invalid arguments!" << std::endl << std::endl;
        PrintUsage(argv[0]);
        return ErrorCode::InvalidArgument;
    }

    if (!PrintArgumentsAndPrompt(args))
    {
        return ErrorCode::UserCancelled;
    }

    const Tunkio::Memory::AutoOptionsHandle options(Tunkio::CreateOptions());
    const Tunkio::Memory::AutoHandle tunkio(TunkioCreate(options.get()));

    if (!tunkio)
    {
        std::cerr << "Failed to create TunkioHandle!" << std::endl;
        return -666; // TODO: FIX
    }

    if (!TunkioRun(tunkio.get()))
    {
        std::cerr << "Tunkio failed." << std::endl;
    }

    return g_error;
}