#include "KuuraAPI-PCH.hpp"
#include "KuuraComposer.hpp"

#include "FillProviders/KuuraFillProvider.hpp"
#include "FillProviders/KuuraByteFiller.hpp"
#include "FillProviders/KuuraSequenceFiller.hpp"
#include "FillProviders/KuuraFileFiller.hpp"
#include "FillProviders/KuuraRandomFiller.hpp"

#include "Workloads/KuuraWorkload.hpp"
#include "Workloads/KuuraFileWorkload.hpp"
#include "Workloads/KuuraDirectoryWorkload.hpp"
#include "Workloads/KuuraDriveWorkload.hpp"

namespace Kuura
{
	constexpr size_t Length(const char* str)
	{
		char* ptr = const_cast<char*>(str);

		if (ptr)
		{
			while (*ptr)
			{
				ptr++;
			}
		}

		return (ptr - str);
	}

	Composer::Composer(void* context) :
		_callbacks(context)
	{
	}

	bool Composer::AddTarget(KuuraTargetType type, const std::filesystem::path& path, bool removeAfterOverwrite)
	{
		switch (type)
		{
			case KuuraTargetType::FileOverwrite:
			{
				_workloads.emplace_back(new Kuura::FileWorkload(&_callbacks, path, removeAfterOverwrite));
				return true;
			}
			case KuuraTargetType::DirectoryOverwrite:
			{
				_workloads.emplace_back(new Kuura::DirectoryWorkload(&_callbacks, path, removeAfterOverwrite));
				return true;
			}
			case KuuraTargetType::DriveOverwrite:
			{
				if (removeAfterOverwrite)
				{
					return false;
				}

				_workloads.emplace_back(new Kuura::DriveWorkload(&_callbacks, path));
				return true;
			}
		}

		return false;
	}

	bool Composer::AddPass(KuuraFillType type, bool verify, const char* optional)
	{
		switch (type)
		{
			case KuuraFillType::ZeroFill:
			{
				_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0x00), verify));
				return true;
			}
			case KuuraFillType::OneFill:
			{
				_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0xFF), verify));
				return true;
			}
			case KuuraFillType::ByteFill:
			{
				if (Length(optional) != 1)
				{
					return false;
				}

				_fillers.emplace_back(new Kuura::ByteFiller(std::byte(optional[0]), verify));
				return true;
			}
			case KuuraFillType::SequenceFill:
			{
				if (Length(optional) < 1)
				{
					return false;
				}

				_fillers.emplace_back(new Kuura::SequenceFiller(optional, verify));
				return true;
			}
			case KuuraFillType::FileFill:
			{
				if (Length(optional) < 1)
				{
					return false;
				}

				auto fileFiller = std::make_shared<Kuura::FileFiller>(optional, verify);

				if (!fileFiller->HasContent())
				{
					return false;
				}

				_fillers.emplace_back(fileFiller);
				return true;
			}
			case KuuraFillType::RandomFill:
			{
				_fillers.emplace_back(new Kuura::RandomFiller(verify));
				return true;
			}
		}

		return false;
	}

	bool Composer::Run()
	{
		for (auto& workload : _workloads)
		{
			if (!workload->Run(_fillers))
			{
				return false;
			}
		}

		return !_workloads.empty();
	}
}
