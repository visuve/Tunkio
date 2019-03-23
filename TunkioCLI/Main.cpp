#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    void PrintUsage(const std::filesystem::path& exe)
    {
        std::cout << " Usage:" << std::endl << std::endl;
        std::cout << "  --path=\"P:\\Path\\To\\File|Disk|Volume\" (Required) " << std::endl;
        std::cout << "  --target=[f|d|m] where f=file, d=directory, m=mass storage device (Optional) " << std::endl;
        std::cout << "  --mode=[0|1|r|R] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random, R=less random (Optional)" << std::endl;
        std::cout << "  --remove=[y|n] remove on exit y=yes, n=no. Applies only to file or directory (Optional)" << std::endl;
        std::cout << std::endl;
        std::cout << " Usage examples:" << std::endl << std::endl;
        std::cout << "  " << exe << " --path=\"C:\\SecretFile.txt\" --target=f --mode=r" << std::endl;
        std::cout << "  " << exe << " --path=\"C:\\SecretDirectory\" --target=d --mode=r" << std::endl;
        std::cout << "  " << exe << " --path=\\\\.\\PHYSICALDRIVE1 --target=v --mode=r" << std::endl;
        std::cout << std::endl;

        std::cout << std::endl << "Here are your volumes:" << std::endl << std::endl;
        int error = system("wmic diskdrive list brief");

        if (error != 0)
        {
            std::cerr << "Listing your drives failed with wmic: " << error << std::endl;
        }
    }

    std::map<std::string, Args::Argument> Arguments =
    {
        { "target", Args::Argument(false, TunkioTarget::AutoDetect) },
        { "mode", Args::Argument(false, TunkioMode::Zeroes) },
        { "remove", Args::Argument(false, false) },
        { "path", Args::Argument(true, std::filesystem::path()) }
    };

    template <typename T, typename C>
    T* Clone(const std::basic_string<C>& str)
    {
        const size_t bytes = str.length() * sizeof(C) + sizeof(C);
        auto buffer = new T[bytes];
        std::memcpy(buffer, str.c_str(), bytes);
        return buffer;
    }

    TunkioOptions* CreateOptions()
    {
        const auto progress = [](uint64_t bytesWritten, uint64_t secondsElapsed) -> void
        {
            assert(bytesWritten && secondsElapsed && "Progress triggered with zero(s)");
            const uint64_t megabytesWritten = bytesWritten / 1024;
            std::cout << megabytesWritten << " megabytes written. Speed " << megabytesWritten / secondsElapsed << " MB/s" << std::endl;
        };

        const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

        return new TunkioOptions
        {
            Arguments.at("target").Value<TunkioTarget>(),
            Arguments.at("mode").Value<TunkioMode>(),
            Arguments.at("remove").Value<bool>(),
            progress,
            TunkioString{ path.size(), Clone<char>(path) }
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
}

int main(int argc, char* argv[])
{
    using namespace Tunkio;

    std::cout << "Tunkio 0.1" << std::endl << std::endl;

    if (argc <= 1)
    {
        std::cerr << "Invalid arguments!" << std::endl << std::endl;
        PrintUsage(argv[0]);
        return ERROR_BAD_ARGUMENTS;
    }

    if (!Args::Parse(Arguments, std::vector<std::string>({ argv + 1, argv + argc })))
    {
        return ERROR_BAD_ARGUMENTS;
    }

    const std::unique_ptr<TunkioOptions, TunkioOptionsDeleter> options(CreateOptions());

    const uint32_t result = TunkioExecute(options.get());

    system("PAUSE"); // TODO: replace with std::cin?

    return result;
}