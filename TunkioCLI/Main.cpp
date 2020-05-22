#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioTime.hpp"
#include "TunkioChildProcess.hpp"
#include "TunkioDataUnits.hpp"
#include "TunkioMemory.hpp"

namespace Tunkio
{
    Time::Timer g_totalTimer;
    Time::Timer g_currentTimer;
    uint32_t g_error = ErrorCode::Success;
    uint64_t g_bytesToWrite = 0;
    uint64_t g_bytesWrittenLastTime = 0;

    void PrintUsage(const std::filesystem::path& exe)
    {
        std::cout << " Usage:" << std::endl << std::endl;
        std::cout << "  --path=\"P:\\Path\\To\\File|Disk|Volume\" (Required) " << std::endl;
        std::cout << "  --target=[f|d|m] where f=file, d=directory, m=mass storage device (Optional) " << std::endl;
        std::cout << "  --mode=[0|1|r] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random (Optional)" << std::endl;
        std::cout << "  --remove=[y|n] remove on exit y=yes, n=no. Applies only to file or directory (Optional)" << std::endl;
        std::cout << std::endl;
        std::cout << " Usage examples:" << std::endl << std::endl;
        std::cout << "  " << exe.string() << " --path=\"C:\\SecretFile.txt\" --target=" << char(TunkioTarget::File) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=\"C:\\SecretDirectory\" --target=" << char(TunkioTarget::Directory) << " --mode=r" << std::endl;
        std::cout << "  " << exe.string() << " --path=\\\\.\\PHYSICALDRIVE9 --target=" << char(TunkioTarget::Device) << " --mode=r" << std::endl;
        std::cout << std::endl;

        ChildProcess wmic(L"C:\\Windows\\System32\\wbem\\WMIC.exe", L"diskdrive list brief");

        if (!wmic.Run())
        {
            std::cerr << "Failed to start wmic.exe to list disk drives. Error code: " << wmic.ErrorCode() << std::endl;
            return;
        }

        if (wmic.ExitCode() != 0)
        {
            std::cerr << "Listing your drives failed with wmic. Exit code: " << wmic.ExitCode() << std::endl;
            return;
        }

        std::cout << std::endl << "Here are your volumes:" << std::endl << std::endl;
        std::cout << wmic.Output() << std::endl;
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

    TunkioOptions* CreateOptions()
    {
        std::cout << std::setprecision(3) << std::fixed;

        const auto started = [](uint64_t bytesLeft) -> void
        {
            g_bytesToWrite = bytesLeft;
        };

        const auto progress = [](uint64_t bytesWritten) -> void
        {
            if (!bytesWritten)
            {
                return;
            }

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
        };

        const auto errors = [](uint32_t error, uint64_t bytesWritten) -> void
        {
            if (bytesWritten)
            {
                std::cerr << "Error " << error << " occurred. Bytes written: " << bytesWritten << std::endl;
            }
            else
            {
                std::cerr << "Error " << error << " occurred." << std::endl;
            }

#ifdef WIN32
            std::string buffer(0x400, 0);
            if (FormatMessageA(
                    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
                    nullptr, 
                    error, 
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    buffer.data(), 
                    static_cast<DWORD>(buffer.size()), 
                    nullptr))
            {
                std::cerr << "Detailed description: " << buffer << std::endl;
            }
#endif

            g_error = error;
        };

        const auto completed = [](uint64_t bytesWritten) -> void
        {
            std::cout << "Finished. Bytes written: " << bytesWritten << std::endl;

            const DataUnit::Byte bytes(bytesWritten);
            const auto elapsed = g_totalTimer.Elapsed<Time::MilliSeconds>();

            if (bytes.Value() && elapsed.count())
            {
                std::cout << "Average speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
            }
        };

        const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

        return new TunkioOptions
        {
            Arguments.at("target").Value<TunkioTarget>(),
            Arguments.at("mode").Value<TunkioMode>(),
            Arguments.at("remove").Value<bool>(),
            TunkioCallbacks { started, progress, errors, completed },
            TunkioString{ path.size(), Memory::CloneString(path) }
        };
    }
}

void SignalHandler(int signal)
{
    std::cout << "Got signal: " << signal << std::endl;
    exit(Tunkio::ErrorCode::UserCancelled);
}

int main(int argc, char* argv[])
{
    using namespace Tunkio;

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
        system("PAUSE");
        return ErrorCode::InvalidArgument;
    }

    auto args = std::vector<std::string>({ argv + 1, argv + argc });

    if (!PrintArgumentsAndPrompt(args))
    {
        return ErrorCode::UserCancelled;
    }

    if (std::signal(SIGINT, SignalHandler) == SIG_ERR)
    {
        std::cerr << "Cannot attach SIGINT handler!" << std::endl;
        return EXIT_FAILURE;
    }

    if (!Args::ParseVector(Arguments, args))
    {
        return ErrorCode::InvalidArgument;
    }

    const Tunkio::Memory::AutoOptionsHandle options(Tunkio::CreateOptions());
    const Tunkio::Memory::AutoHandle tunkio(TunkioCreate(options.get()));

    if (!tunkio)
    {
        std::cerr << "Failed to create TunkioHandle!" << std::endl;
        return -666; // TODO: FIX
    }

    const DWORD result = TunkioRun(tunkio.get());

    if (result != Tunkio::ErrorCode::Success)
    {
        std::cerr << "FAILED: " << result << std::endl;
        return result;
    }

    return g_error;
}