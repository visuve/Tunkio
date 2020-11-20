#include "TunkioCLI-PCH.hpp"
#include "TunkioCLI.hpp"

namespace Tunkio
{
	CLI g_cli;

	void SignalHandler(int signal)
	{
		std::cout << "Got signal: " << signal << std::endl;
		g_cli.Stop();
	}

	void PrintUsage(const std::string& exe)
	{
		std::cout << " Usage:" << std::endl << std::endl;
#if defined(_WIN32)
		std::cout << R"(  --path="P:\Path\To\File or Drive" (Required))" << std::endl;
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
		std::cout << "  " << exe << R"( --path="C:\\SecretFile.txt" --target=f --mode=r)" << std::endl;
		std::cout << "  " << exe << R"( --path="C:\\SecretDirectory" --target=d --mode=r)" << std::endl;
		std::cout << "  " << exe << R"( --path=\\.\PHYSICALDRIVE9 --target=D --mode=r)" << std::endl;
#else
		std::cout << "  " << exe << " --path=/home/you/secret_file.txt --target=f --mode=r" << std::endl;
		std::cout << "  " << exe << " --path=/home/you/secret_directory --target=d --mode=r" << std::endl;
		std::cout << "  " << exe << " --path=/dev/sdx --target= --target=D --mode=r" << std::endl;
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
		std::cout << "Are you sure you want to continue? [y/n]" << std::endl;

		const int prompt = getchar();
		std::cout << std::endl;
		return prompt == 'y' || prompt == 'Y';
	}

	std::map<std::string, Args::Argument> Arguments =
	{
		{ "path", Args::Argument(true, std::string()) },
		{ "target", Args::Argument(false, TunkioTargetType::File) },
		{ "mode", Args::Argument(false, TunkioFillType::Zeroes) },
		{ "filler", Args::Argument(false, std::string()) },
		{ "verify", Args::Argument(false, false) },
		{ "remove", Args::Argument(false, false) }
	};

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
		else if (mode == TunkioFillType::Sentence)
		{
			if (filler.empty())
			{
				std::cerr << "Sentence argument needs to be at least one character!" << std::endl << std::endl;
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

	if (std::signal(SIGINT, Tunkio::SignalHandler) == SIG_ERR)
	{
		std::cerr << "Cannot attach CTRL+C interrupt handler!" << std::endl;
		return ErrorCode::Generic;
	}

	// TODO: merge these into one
	{
		const std::vector<std::string> args({ argv + 1, argv + argc });

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
	}

	if (!g_cli.Initialize(
		Arguments.at("path").Value<std::string>(),
		Arguments.at("target").Value<TunkioTargetType>(),
		Arguments.at("mode").Value<TunkioFillType>(),
		Arguments.at("filler").Value<std::string>(),
		Arguments.at("remove").Value<bool>(),
		Arguments.at("verify").Value<bool>()))
	{
		std::cerr << "Tunkio failed to initialize." << std::endl;
		return ErrorCode::Generic;
	}

	if (!g_cli.Run())
	{
		std::cerr << "Tunkio failed." << std::endl;
		return g_cli.Error();
	}

	return ErrorCode::Success;
}
