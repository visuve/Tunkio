#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"

namespace Tunkio::GUI
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

	bool OnProgress(uint64_t bytesWritten)
	{
		if (!g_dialog)
		{
			std::cerr << "Dialog is null" << std::endl;
			return false;
		}

		return g_dialog->OnProgress(bytesWritten);
	}

	void OnErrors(TunkioStage stage, uint64_t bytesWritten, uint32_t error)
	{
		if (!g_dialog)
		{
			std::cerr << "Dialog is null" << std::endl;
			return;
		}

		g_dialog->OnErrors(stage, bytesWritten, error);
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
		{ "target", Args::Argument(false, TunkioTargetType::File) },
		{ "mode", Args::Argument(false, TunkioFillMode::Zeroes) },
		{ "remove", Args::Argument(false, false) },
		{ "path", Args::Argument(true, std::filesystem::path()) }
	};

	TunkioOptions* CreateOptions()
	{
		std::cout << std::setprecision(3) << std::fixed;

		const auto path = Arguments.at("path").Value<std::filesystem::path>().string();

		return new TunkioOptions
		{
			Arguments.at("target").Value<TunkioTargetType>(),
			Arguments.at("mode").Value<TunkioFillMode>(),
			Arguments.at("remove").Value<bool>(),
			TunkioCallbacks { OnStarted, OnProgress, OnErrors, OnCompleted },
			TunkioString { static_cast<uint32_t>(path.size()), Memory::CloneString(path) }
		};
	}

	void Usage()
	{
		nana::msgbox mb("Tunkio GUI");
		mb << "This program is intended to be run through you OS's context menus." << std::endl << std::endl;;
		mb << "However, you can call this application trough command line, ";
		mb << "see TunkioCLI for example usage. Same parameters apply for this GUI.";
		mb.show();
		nana::exec();
	}

	int Run()
	{
		const Tunkio::Memory::AutoOptionsHandle options(CreateOptions());
		const Tunkio::Memory::AutoHandle tunkio(TunkioCreate(options.get()));

		if (!tunkio)
		{
			std::cerr << "Failed to create TunkioHandle!" << std::endl;
			return -666; // TODO: FIX
		}

		ProgressDialog progressDialog("TODO", tunkio.get());
		g_dialog = &progressDialog;

		progressDialog.Show();
		nana::exec();

		return 0;
	}
}

#if defined(_WIN32) || defined(_WIN64)
int WINAPI WinMain(HINSTANCE, HINSTANCE, char* cmdLine, int)
{
	if (!Tunkio::Args::ParseString(Tunkio::GUI::Arguments, cmdLine))
	{
		Tunkio::GUI::Usage();
		return -1;
	}

	return Tunkio::GUI::Run();
}
#else
int main(int argc, char** argv)
{
	const std::vector<std::string> args({ argv + 1, argv + argc });

	if (!Tunkio::Args::ParseVector(Tunkio::GUI::Arguments, args))
	{
		Tunkio::GUI::Usage();
		return -1;
	}

	return Tunkio::GUI::Run();
}
#endif