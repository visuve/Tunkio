#include "PCH.hpp"
#include "TunkioArgs.hpp"
#include "TunkioAPI.h"

namespace Tunkio
{
    void PrintUsage(const std::wstring& exe)
    {
        std::wcout << L" Usage:" << std::endl << std::endl;
        std::wcout << L"  --path=\"P:\\Path\\To\\File|Disk|Volume\" (Required) " << std::endl;
        std::wcout << L"  --target=[a|f|d|v] where a=automatic, f=file, d=directory, v=volume (Optional) " << std::endl;
        std::wcout << L"  --mode=[0|1|r|R] where overwrite mode 0=fill with zeros, 1=fill with ones, r=random, R=less random (Optional)" << std::endl;
        std::wcout << std::endl;
        std::wcout << L" Usage examples:" << std::endl << std::endl;
        std::wcout << L"  " << exe << L" --path=\"C:\\SecretFile.txt\" --target=f --mode=r"  << std::endl;
        std::wcout << L"  " << exe << L" --path=\"C:\\SecretDirectory\" --target=d --mode=r" << std::endl;
        std::wcout << L"  " << exe << L" --path=\\\\.\\PHYSICALDRIVE1 --target=v --mode=r" << std::endl;
        std::wcout << std::endl;

        std::wcout << std::endl << "Here are your volumes:" << std::endl << std::endl;
        int error = _wsystem(L"wmic diskdrive list brief");

        if (error != 0)
        {
            std::wcerr << L"Listing your drives failed with wmic: " << error << std::endl;
        }
    }

    std::array<Args::Argument, 3> Args::Arguments =
    {
        Args::Argument(true, L"--path=", std::wstring()),
        Args::Argument(false, L"--target=", Args::Target::AutoDetect),
        Args::Argument(false, L"--mode=", Args::Mode::Zeroes)
    };
}

int wmain(int argc, wchar_t* argv[])
{
    std::wcout << L"Tunkio 0.1" << std::endl << std::endl;

    if (argc <= 1 || !Tunkio::Args::Parse({ argv + 1, argv + argc }))
    {
        std::wcerr << L"Invalid arguments!" << std::endl << std::endl;
        Tunkio::PrintUsage(argv[0]);
        return ERROR_BAD_ARGUMENTS;
    }

    return TunkioExecuteW(argv[1], L'v', L'r');
}