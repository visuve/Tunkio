#include "../KuuraLIB-PCH.hpp"
#include "../KuuraDriveInfo.hpp"

#include <libgeom.h>

namespace Kuura
{
	class GeomTree
	{
	public:
		GeomTree()
		{
			m_mesh = std::make_unique<gmesh>();

			if (geom_gettree(m_mesh.get()) != 0)
			{
				std::cerr << "Cannot get GEOM tree" << std::endl;
			}
		}

		~GeomTree()
		{
			geom_deletetree(m_mesh.get());
			m_mesh.reset();
		}

		gclass* find_geom_class(const std::string& name)
		{
			if (!m_mesh)
			{
				return nullptr;
			}

			for (gclass* iter = m_mesh->lg_class.lh_first;
				iter;
				iter = iter->lg_class.le_next)
			{
				if (name.compare(iter->lg_name) == 0)
				{
					return iter;
				}
			}

			std::cerr << "Class: " << name << " not found" << std::endl;
			return nullptr;
		}

	private:
		std::unique_ptr<gmesh> m_mesh;
	};

	void FillDriveInfo(ggeom* diskObject, Drive& drive)
	{
		if (!diskObject)
		{
			std::cerr << "Invalid disk object!" << std::endl;
			return;
		}

		for (gprovider* provider = diskObject->lg_provider.lh_first;
			provider;
			provider = provider->lg_provider.le_next)
		{
			drive.Path = provider->lg_name;
			drive.Capacity = static_cast<uint64_t>(provider->lg_mediasize);

			for (gconfig* config = provider->lg_config.lh_first;
				config;
				config = config->lg_config.le_next)
			{
				if (std::string("descr").compare(config->lg_name) == 0)
				{
					drive.Description = config->lg_val;
				}
			}
		}
	}

	void FillPartitionInfo(ggeom* partitionObject, Drive& drive)
	{
		if (!partitionObject)
		{
			std::cerr << "Invalid partition object!" << std::endl;
			return;
		}

		if (drive.Path.compare(partitionObject->lg_name) != 0)
		{
			return;
		}

		for (gprovider* provider = partitionObject->lg_provider.lh_first;
			provider;
			provider = provider->lg_provider.le_next)
		{
			++drive.Partitions;
		}
	}

	std::vector<Drive> DriveInfo()
	{
		GeomTree tree;

		gclass* diskClass = tree.find_geom_class("DISK");

		if (!diskClass)
		{
			return {};
		}

		std::vector<Drive> drives;

		for (ggeom* diskObject = diskClass->lg_geom.lh_first;
			diskObject;
			diskObject = diskObject->lg_geom.le_next)
		{
			Drive drive;
			FillDriveInfo(diskObject, drive);
			drives.emplace_back(drive);
		}

		gclass* partitionClass = tree.find_geom_class("PART");

		if (!partitionClass)
		{
			return {};
		}

		for (ggeom* partitionObject = partitionClass->lg_geom.lh_first;
			partitionObject;
			partitionObject = partitionObject->lg_geom.le_next)
		{
			for (Drive& drive : drives)
			{
				FillPartitionInfo(partitionObject, drive);
			}
		}

		for (Drive& drive : drives)
		{
			const char* fullPath = g_device_path(drive.Path.c_str());

			if (fullPath)
			{
				drive.Path = fullPath;
			}
		}

		return drives;
	}
}
