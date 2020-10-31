#include "PCH.hpp"
#include "TunkioProgressDialog.hpp"
#include "TunkioErrorCodes.hpp"

namespace Tunkio::GUI
{
	ProgressDialog* g_dialog = nullptr;
	uint32_t g_error = ErrorCode::Success;

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

	void OnError(TunkioStage stage, uint64_t bytesWritten, uint32_t error)
	{
		g_error = error;

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
		{ "path", Args::Argument(true, std::string()) }
	};

	void Usage()
	{
		nana::msgbox mb("Tunkio GUI");
		mb << "This program is intended to be run through you OS's context menus." << std::endl << std::endl;;
		mb << "However, you can call this application trough command line, ";
		mb << "see TunkioCLI for example usage. Same parameters apply for this GUI.";
		mb.show();
		nana::exec();
	}

	int Run(const std::string& title)
	{
		const Tunkio::Memory::AutoHandle tunkio(TunkioInitialize(
			Arguments.at("path").Value<std::string>().c_str(),
			Arguments.at("target").Value<TunkioTargetType>()));

		if (!tunkio)
		{
			std::cerr << "Failed to create TunkioHandle!" << std::endl;
			return ErrorCode::InvalidArgument;
		}

		if (!TunkioSetFillMode(tunkio.get(), Arguments.at("mode").Value<TunkioFillMode>()) ||
			!TunkioSetStartedCallback(tunkio.get(), OnStarted) ||
			!TunkioSetProgressCallback(tunkio.get(), OnProgress) ||
			!TunkioSetCompletedCallback(tunkio.get(), OnCompleted) ||
			!TunkioSetErrorCallback(tunkio.get(), OnError) ||
			!TunkioSetRemoveAfterFill(tunkio.get(), Arguments.at("remove").Value<bool>()))
		{
			std::cerr << "Failed to pass arguments!" << std::endl;
			return ErrorCode::InvalidArgument;
		}

		ProgressDialog progressDialog(title, tunkio.get());
		g_dialog = &progressDialog;

		progressDialog.Show();
		nana::exec();

		return g_error;
	}
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE, HINSTANCE, char* cmdLine, int)
{
	if (!Tunkio::Args::ParseString(Tunkio::GUI::Arguments, cmdLine))
	{
		Tunkio::GUI::Usage();
		return Tunkio::ErrorCode::InvalidArgument;
	}

	return Tunkio::GUI::Run(cmdLine);
}
#else
int main(int argc, char** argv)
{
	const std::vector<std::string> args({ argv + 1, argv + argc });

	auto join = [](std::string a, std::string b)
	{
		return a + ' ' + b;
	};

	const std::string cmdLine =
		std::accumulate(std::next(args.begin()), args.end(), args.front(), join);

	if (!Tunkio::Args::ParseVector(Tunkio::GUI::Arguments, args))
	{
		Tunkio::GUI::Usage();
		return Tunkio::ErrorCode::InvalidArgument;
	}

	return Tunkio::GUI::Run(cmdLine);
}
#endif