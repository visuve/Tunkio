#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

namespace Tunkio
{
    ProgressDialog* g_dialog = nullptr;

    void OnStarted(uint64_t bytesLeft)
    {
        if (!g_dialog)
        {
            std::cerr << "Dialog is null" << std::endl;
            return;
        }

        g_dialog->OnStarted(bytesLeft);
    }

    void OnProgress(uint64_t bytesWritten)
    {
        if (!g_dialog)
        {
            std::cerr << "Dialog is null" << std::endl;
            return;
        }

        g_dialog->OnProgress(bytesWritten);
    }

    void OnErrors(uint32_t error, uint64_t bytesWritten)
    {
        if (!g_dialog)
        {
            std::cerr << "Dialog is null" << std::endl;
            return;
        }

        g_dialog->OnErrors(error, bytesWritten);
    }

    void OnCompleted(uint64_t bytesWritten)
    {
        if (!g_dialog)
        {
            std::cerr << "Dialog is null" << std::endl;
            return;
        }

        g_dialog->OnCompleted(bytesWritten);
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

        const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

        return new TunkioOptions
        {
            Arguments.at("target").Value<TunkioTarget>(),
            Arguments.at("mode").Value<TunkioMode>(),
            Arguments.at("remove").Value<bool>(),
            TunkioCallbacks { OnStarted, OnProgress, OnErrors, OnCompleted },
            TunkioString{ path.size(), Memory::CloneString(path) }
        };
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, char* cmdline, int)
{
    if (!Tunkio::Args::ParseString(Tunkio::Arguments, cmdline))
    {
        return -1;
    }

    const Tunkio::Memory::AutoOptionsHandle options(Tunkio::CreateOptions());
    const Tunkio::Memory::AutoHandle tunkio(TunkioCreate(options.get()));

    if (!tunkio)
    {
        std::cerr << "Failed to create TunkioHandle!" << std::endl;
        return -666; // TODO: FIX
    }

    Tunkio::ProgressDialog progressDialog(cmdline, tunkio.get());
    progressDialog.Show();

    Tunkio::g_dialog = &progressDialog;

    nana::exec();

    return 0;
}