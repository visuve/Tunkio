#include "KuuraCLI-PCH.hpp"
#include "KuuraCLI.hpp"
#include "KuuraArgument.hpp"
#include "../KuuraResources/KuuraVersion.h"

namespace Kuura
{
	CLI _cli;

	std::map<std::string, Argument> Arguments =
	{
		{ "target", Argument(true, KuuraTargetType::FileOverwrite) },
		{ "path", Argument(true, std::filesystem::path()) },
		{ "mode", Argument(false, KuuraFillType::ZeroFill) },
		{ "filler", Argument(false, std::string()) },
		{ "verify", Argument(false, false) },
		{ "remove", Argument(false, false) },
		{ "prompt", Argument(false, true) }
	};

	void SignalHandler(int signal)
	{
		std::cout << "Got signal: " << signal << std::endl;
		_cli.Stop();
	}

	void PrintUsage(const std::filesystem::path& exe)
	{
		std::cout << " Usage:" << std::endl << std::endl;
#if defined(_WIN32)
		std::cout << R"(  --path="P:\Path\To\File or Drive" (Required))" << std::endl;
#else
		std::cout << "  --path=/path/to/file_or_device (Required)" << std::endl;
#endif
		std::cout << "  --target=[f|d|D] where f=file, d=directory, D=drive " << std::endl;

		std::cout << "  --mode=[0|1|r|c|s|f] where "
			<< "0=fill with zeros, 1=fill with ones, r=random, b=byte, s=sequence, f=filepath."
			<< " Note: with option 'b', 's' or 'f' you need to provide filler argument." << std::endl;

		std::cout << "  --remove=[y|n] remove after wipe "
			<< "y=yes, n=no. Applies only to file or directory. Default=no" << std::endl;

		std::cout << "  --filler=[character|string|filepath] "
			<< "required when --mode is c, s or f." << std::endl;

		std::cout << "  --verify=[y|n] "
			<< "verify overwritten data. Default=no" << std::endl;

		std::cout << "  --prompt=[y|n] "
			<< "prompt before beginning overwrite. Default=yes" << std::endl;

		std::cout << std::endl;
		std::cout << " Usage examples:" << std::endl << std::endl;
#if defined(_WIN32)
		std::cout << "  " << exe.filename().string()
			<< R"( --path="C:\\Users\\You\\SecretFile.txt" --target=f)" << std::endl;
		std::cout << "  " << exe.filename().string()
			<< R"( --path="C:\\Users\\You\\SecretDirectory" --target=d)" << std::endl;
		std::cout << "  " << exe.filename().string()
			<< R"( --path=\\.\PHYSICALDRIVE9 --target=D)" << std::endl;
#else
		std::cout << "  ./" << exe.filename().string()
			<< " --path=/home/you/secret_file.txt --target=f" << std::endl;
		std::cout << "  ./" << exe.filename().string()
			<< " --path=/home/you/secret_directory --target=d" << std::endl;
		std::cout << "  ./" << exe.filename().string()
			<< " --path=/dev/sdx --target=D" << std::endl;
#endif
		std::cout << std::endl;

		std::vector<Drive> drives = DriveInfo();

		if (drives.empty())
		{
			return;
		}

		std::cout << std::endl << " Here are your drives:" << std::endl << std::endl;

		std::cout.width(24);
		std::cout << std::left << " Path:";

		std::cout.width(38);
		std::cout << "Description:";

		std::cout.width(15);
		std::cout << "Partitions:";

		std::cout << "Capacity:" << std::endl;

		for (const auto& drive : drives)
		{
			std::cout << "  ";
			std::cout.width(23);
			std::cout << drive.Path;

			std::cout.width(38);
			std::cout << drive.Description;

			std::cout.width(15);
			std::cout << drive.Partitions;

			std::cout << drive.Capacity << std::endl;
		}
	}

	bool PrintArgumentsAndPrompt(const std::vector<std::string>& args)
	{
		const auto accumulator = [](const std::string& accumulated, const std::string& value)
		{
			return value.empty() ? accumulated : accumulated + "\n  " + value;
		};

		const std::string joined =
			std::accumulate(args.cbegin(), args.cend(), std::string(), accumulator);

		std::cout << "Provided arguments: " << joined << std::endl;

		if (!Arguments.at("prompt").Value<bool>())
		{
			return true;
		}

		std::cout << "Are you sure you want to continue? [y/n]" << std::endl;

		const int prompt = getchar();
		std::cout << std::endl;
		return prompt == 'y' || prompt == 'Y';
	}

	bool CheckArguments()
	{
		auto mode = Arguments.at("mode").Value<KuuraFillType>();
		auto filler = Arguments.at("filler").Value<std::string>();

		if (mode == KuuraFillType::ByteFill)
		{
			if (filler.size() != 1)
			{
				std::cerr << "Filler argument needs to be one character!" << std::endl << std::endl;
				return false;
			}
		}
		else if (mode == KuuraFillType::SequenceFill)
		{
			if (filler.empty())
			{
				std::cerr << "Sequence argument needs to be at least one character!" << std::endl << std::endl;
				return false;
			}
		}
		else if (mode == KuuraFillType::FileFill)
		{
			if (!std::filesystem::exists(filler))
			{
				std::cerr << "Please use a valid path with file fill!" << std::endl << std::endl;
				return false;
			}
		}
		else if (!filler.empty())
		{
			std::cerr << "Filler argument can only be used with modes c, s or f!" << std::endl << std::endl;
			return false;
		}

		return true;
	}
}

int main(int argc, char* argv[])
{
	using namespace Kuura;

	std::cout << std::endl;
	std::cout << " #  #  #   #  #   #  ###     ###" << std::endl;
	std::cout << " # #   #   #  #   #  #  #   #   #" << std::endl;
	std::cout << " ##    #   #  #   #  # #    #####" << std::endl;
	std::cout << " # #   #   #  #   #  #  #   #   #"<< std::endl;
	std::cout << " #  #   ###    ###   #   #  #   #" << std::endl;
	std::cout << std::endl << "Version " << KuuraVersionString << std::endl << std::endl;

	if (argc <= 1)
	{
		std::cerr << "Too few arguments!" << std::endl << std::endl;
		PrintUsage(argv[0]);
		return ErrorCode::InvalidArgument;
	}

	if (std::signal(SIGINT, SignalHandler) == SIG_ERR)
	{
		std::cerr << "Cannot attach CTRL+C interrupt handler!" << std::endl;
		return ErrorCode::Generic;
	}

	// TODO: merge these into one
	{
		const std::vector<std::string> args({ argv + 1, argv + argc });

		if (!ParseVector(Arguments, args))
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
	}

	if (!_cli.Initialize(
		Arguments.at("target").Value<KuuraTargetType>(),
		Arguments.at("path").Value<std::filesystem::path>(),
		Arguments.at("mode").Value<KuuraFillType>(),
		Arguments.at("filler").Value<std::string>(),
		Arguments.at("verify").Value<bool>(),
		Arguments.at("remove").Value<bool>()))
	{
		std::cerr << "Kuura failed to initialize." << std::endl;
		return ErrorCode::Generic;
	}

	if (!_cli.Run())
	{
		std::cerr << "Kuura failed." << std::endl;
		return _cli.Error();
	}

	return ErrorCode::Success;
}
