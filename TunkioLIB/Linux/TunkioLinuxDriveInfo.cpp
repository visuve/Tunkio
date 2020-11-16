#include "../TunkioLIB-PCH.hpp"
#include "../TunkioDriveInfo.hpp"

#include <libudev.h> // apt install libudev-dev

namespace Tunkio
{
	class UDeviceContext // User space device system context
	{
	public:
		UDeviceContext()
		{
			if (!m_context)
			{
				std::cerr << "Could not create udev context!" << std::endl;
				return;
			}
		}

		~UDeviceContext()
		{
			for (udev_enumerate* enumerator : m_enumerators)
			{
				udev_enumerate_unref(enumerator);
			}

			if (m_context)
			{
				udev_unref(m_context);
			}
		}

		operator udev* () const
		{
			return m_context;
		}

		udev_enumerate* CreateEnumerator()
		{
			udev_enumerate* enumerator = udev_enumerate_new(m_context);

			if (enumerator)
			{
				return m_enumerators.emplace_back(enumerator);
			}

			std::cerr << "Could not create enumerator!" << std::endl;
			return nullptr;
		}

	private:
		udev* m_context = udev_new();
		std::vector<udev_enumerate*> m_enumerators;
	};

	class UDevice
	{
	public:
		UDevice(UDeviceContext& context, udev_list_entry* device_list_entry) :
			m_context(context)
		{
			const char* path = udev_list_entry_get_name(device_list_entry);

			if (!path)
			{
				std::cerr << "Failed to get device path!" << std::endl;
				return;
			}

			m_devicePath = path;
			m_device = udev_device_new_from_syspath(context, path);

			if (!m_device)
			{
				std::cerr << "Failed to get device from path: '"
					<< m_devicePath << "'!" << std::endl;
				return;
			}
		}

		~UDevice()
		{
			if (m_device)
			{
				udev_device_unref(m_device);
			}
		}

		operator udev_device* () const
		{
			return m_device;
		}

		std::string DevicePath() const
		{
			return m_devicePath;
		}

		std::string DeviceType() const
		{
			const char* type = udev_device_get_devtype(m_device);

			if (!type)
			{
				std::cerr << "Failed to get device type from path: '"
					<< m_devicePath << "'!" << std::endl;
				return {};
			}

			return type;
		}

		std::string SystemName() const
		{
			const char* system_name = udev_device_get_sysname(m_device);

			if (!system_name)
			{
				std::cerr << "Failed to get system name from path: '"
					<< m_devicePath << "'!" << std::endl;
				return {};
			}

			return system_name;
		}

		udev_list_entry* Partitions()
		{
			udev_enumerate* enumerator = m_context.CreateEnumerator();
			udev_enumerate_add_match_parent(enumerator, m_device);
			udev_enumerate_add_match_property(enumerator, "DEVTYPE", "partition");
			udev_enumerate_scan_devices(enumerator);
			return udev_enumerate_get_list_entry(enumerator);
		}

		bool IsLoopDevice() const
		{
			return SystemName().compare(0, 4, "loop") == 0;
		}

	private:
		UDeviceContext& m_context;
		udev_device* m_device = nullptr;
		std::string m_devicePath;
	};


	void FillDriveInfo(const UDevice& device, Drive& di)
	{
		const char* path = udev_device_get_devnode(device);

		if (!path)
		{
			std::cerr << "Failed to get file system path from: '"
				<< device.DevicePath() << "'!" << std::endl;
		}
		else
		{
			di.Path = path;
		}

		const char* description = udev_device_get_property_value(device, "ID_MODEL");

		if (!description)
		{
			std::cerr << "Failed to get ID_MODEL from: '"
				<< device.DevicePath() << "'!" << std::endl;
		}
		else
		{
			di.Description = description;
		}


		// https://people.redhat.com/msnitzer/docs/io-limits.txt
		const char* sizeStr = udev_device_get_sysattr_value(device, "size");
		const char* blockSizeStr = udev_device_get_sysattr_value(device, "queue/logical_block_size");

		if (!sizeStr)
		{
			std::cerr << "Failed to get size from: '"
				<< device.DevicePath() << "'!" << std::endl;
		}
		else if (!blockSizeStr)
		{
			std::cerr << "Failed to get block size from: '"
				<< device.DevicePath() << "'!" << std::endl;
		}
		else
		{
			uint64_t size = strtoull(sizeStr, nullptr, 10);
			uint64_t blockSize = strtoull(blockSizeStr, nullptr, 10);
			assert(size && blockSize);
			di.Capacity = size * blockSize;
		}

	}

	std::vector<Drive> DriveInfo()
	{
		UDeviceContext context;
		udev_enumerate* enumerator = context.CreateEnumerator();

		if (!enumerator)
		{
			return {};
		}

		udev_enumerate_add_match_subsystem(enumerator, "block");
		udev_enumerate_scan_devices(enumerator);

		udev_list_entry* deviceList = udev_enumerate_get_list_entry(enumerator);

		std::vector<Drive> result;

		for (udev_list_entry* deviceIter = deviceList;
			deviceIter;
			deviceIter = udev_list_entry_get_next(deviceIter))
		{
			UDevice device(context, deviceIter);

			if (device.DeviceType() != "disk" || device.IsLoopDevice())
			{
				continue;
			}

			Drive drive;
			FillDriveInfo(device, drive);

			for (udev_list_entry* partitionIter = device.Partitions();
				partitionIter;
				partitionIter = udev_list_entry_get_next(partitionIter))
			{
				++drive.Partitions;
			}

			result.emplace_back(drive);
		}

		return result;
	}
}
