#include "KuuraAPI-PCH.hpp"
#include "KuuraComposer.hpp"

#include "FillProviders/KuuraFillProvider.hpp"
#include "FillProviders/KuuraByteFiller.hpp"
#include "FillProviders/KuuraSequenceFiller.hpp"
#include "FillProviders/KuuraFileFiller.hpp"
#include "FillProviders/KuuraRandomFiller.hpp"

#include "Workloads/KuuraWorkload.hpp"
#include "Workloads/KuuraFileOverwrite.hpp"
#include "Workloads/KuuraDirectoryOverwrite.hpp"
#include "Workloads/KuuraDriveOverwrite.hpp"

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
		Callbacks(context)
	{
	}

	bool Composer::AddWorkload(const std::filesystem::path& path, KuuraTargetType type, bool removeAfterOverwrite)
	{
		switch (type)
		{
			case KuuraTargetType::FileOverwrite:
			{
				m_workloads.emplace_back(new Kuura::FileOverwrite(this, path, removeAfterOverwrite));
				return true;
			}
			case KuuraTargetType::DirectoryOverwrite:
			{
				m_workloads.emplace_back(new Kuura::DirectoryOverwrite(this, path, removeAfterOverwrite));
				return true;
			}
			case KuuraTargetType::DriveOverwrite:
			{
				if (removeAfterOverwrite)
				{
					return false;
				}

				m_workloads.emplace_back(new Kuura::DriveOverwrite(this, path));
				return true;
			}
		}

		return false;
	}

	bool Composer::AddFiller(KuuraFillType type, bool verify, const char* optional)
	{
		switch (type)
		{
			case KuuraFillType::ZeroFill:
			{
				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0x00), verify));
				return true;
			}
			case KuuraFillType::OneFill:
			{
				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(0xFF), verify));
				return true;
			}
			case KuuraFillType::ByteFill:
			{
				if (Length(optional) != 1)
				{
					return false;
				}

				m_fillers.emplace_back(new Kuura::ByteFiller(std::byte(optional[0]), verify));
				return true;
			}
			case KuuraFillType::SequenceFill:
			{
				if (Length(optional) < 1)
				{
					return false;
				}

				m_fillers.emplace_back(new Kuura::SequenceFiller(optional, verify));
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

				m_fillers.emplace_back(fileFiller);
				return true;
			}
			case KuuraFillType::RandomFill:
			{
				m_fillers.emplace_back(new Kuura::RandomFiller(verify));
				return true;
			}
		}

		return false;
	}

	bool Composer::Run()
	{
		for (auto& workload : m_workloads)
		{
			if (!workload->Run())
			{
				return false;
			}
		}

		return !m_workloads.empty();
	}

	const std::vector<std::shared_ptr<IWorkload>>& Composer::Workloads() const
	{
		return m_workloads;
	}

	const std::vector<std::shared_ptr<IFillProvider>>& Composer::Fillers() const
	{
		return m_fillers;
	}
}
