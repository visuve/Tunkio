#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioErrorCodes.hpp"
#include "TunkioTime.hpp"
#include "TunkioAPI.h"
#include "TunkioChildProcess.hpp"
#include "TunkioDataUnits.hpp"

namespace Tunkio
{
    Time::Timer g_timer;
    uint32_t g_error = ErrorCode::Success;

    void PrintUsage(const std::filesystem::path& exe)
    {
        std::cout << " Usage:" << std::endl << std::endl;
        std::cout << "  --path=\"P:\\Path\\To\\File|Disk|Volume\" (Required) " << std::endl;
        std::cout << "  --target=[f|d|m] where f=file, d=directory, m=mass storage device (Optional) " << std::endl;
        std::cout << "  --mode=[0|1|r] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random (Optional)" << std::endl;
        std::cout << "  --remove=[y|n] remove on exit y=yes, n=no. Applies only to file or directory (Optional)" << std::endl;
        std::cout << std::endl;
        std::cout << " Usage examples:" << std::endl << std::endl;
        std::cout << "  " << exe << " --path=\"C:\\SecretFile.txt\" --target=" << char(TunkioTarget::File) << " --mode=r" << std::endl;
        std::cout << "  " << exe << " --path=\"C:\\SecretDirectory\" --target=" << char(TunkioTarget::Directory) << " --mode=r" << std::endl;
        std::cout << "  " << exe << " --path=\\\\.\\PHYSICALDRIVE9 --target=" << char(TunkioTarget::Device) << " --mode=r" << std::endl;
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

    std::map<std::string, Args::Argument> Arguments =
    {
        { "target", Args::Argument(false, TunkioTarget::File) },
        { "mode", Args::Argument(false, TunkioMode::Zeroes) },
        { "remove", Args::Argument(false, false) },
        { "path", Args::Argument(true, std::filesystem::path()) }
    };

    template <typename T>
    const T* Clone(const std::basic_string<T>& str)
    {
        const size_t bytes = str.length() * sizeof(T) + sizeof(T);
        return reinterpret_cast<const T*>(memcpy(new T[bytes], str.c_str(), bytes));
    }

    TunkioOptions* CreateOptions()
    {
        std::cout << std::setprecision(3) << std::fixed;

        const auto progress = [](uint64_t bytesWritten) -> void
        {
            if (!bytesWritten)
            {
                return;
            }

            const DataUnit::Byte bytes(bytesWritten);
            const auto elapsed = g_timer.Elapsed<Time::MilliSeconds>();

            if (bytes.Value() && elapsed.count())
            {
                std::cout << DataUnit::HumanReadable(bytes) << " written. Speed: " << DataUnit::SpeedPerSecond(bytes, elapsed) << std::endl;
            }
        };

        const auto errors = [](uint32_t error, uint64_t bytesWritten) -> void
        {
            if (bytesWritten)
            {
                std::cout << "Error " << error << "  occurred. Bytes written: " << bytesWritten << std::endl;
            }
            else
            {
                std::cout << "Wipe failed. Error. " << error << std::endl;
            }

            g_error = error;
        };

        const auto completed = [](uint64_t bytesWritten) -> void
        {
            std::cout << "Finished. Bytes written: " << bytesWritten << std::endl;

            const DataUnit::Byte bytes(bytesWritten);
            const auto elapsed = g_timer.Elapsed<Time::MilliSeconds>();

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
            TunkioCallbacks { progress, errors, completed },
            TunkioString{ path.size(), Clone(path) }
        };
    }

    struct TunkioOptionsDeleter
    {
        void operator()(TunkioOptions* options)
        {
            if (options)
            {
                if (options->Path.Data)
                {
                    delete[] options->Path.Data;
                }

                delete options;
            }
        }
    };

    struct TunkioDeleter
    {
        void operator()(TunkioHandle* tunkio)
        {
            if (tunkio)
            {
                TunkioFree(tunkio);
            }
        }
    };
}

int main(int argc, char* argv[])
{
    using namespace Tunkio;

    std::cout << "Tunkio 0.1" << std::endl << std::endl;

    if (argc <= 1)
    {
        std::cerr << "Invalid arguments!" << std::endl << std::endl;
        PrintUsage(argv[0]);
        return ErrorCode::InvalidArgument;
    }

    if (!Args::Parse(Arguments, std::vector<std::string>({ argv + 1, argv + argc })))
    {
        return ErrorCode::InvalidArgument;
    }

    const std::unique_ptr<TunkioOptions, TunkioOptionsDeleter> options(CreateOptions());
    const std::unique_ptr<TunkioHandle, TunkioDeleter> tunkio(TunkioCreate(options.get()));

    if (!tunkio)
    {
        std::cerr << "Failed to create TunkioHandle!" << std::endl;
        return -666; // TODO: FIX
    }

    DWORD result = TunkioRun(tunkio.get());

    system("PAUSE");

    if (result != Tunkio::ErrorCode::Success)
    {
        std::cerr << "FAILED: " << result << std::endl;
        return result;
    }

    return g_error;
}