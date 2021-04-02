#include "../KuuraLIB-PCH.hpp"
#include "../KuuraDriveInfo.hpp"

#include <libudev.h> // apt install libudev-dev OR sudo dnf install systemd-devel

namespace Kuura
{
	class UDeviceContext // User space device system context
	{
	public:
		UDeviceContext()
		{
			if (!_context)
			{
				std::cerr << "Could not create udev context!" << std::endl;
				return;
			}
		}

		~UDeviceContext()
		{
			for (udev_enumerate* enumerator : _enumerators)
			{
				udev_enumerate_unref(enumerator);
			}

			if (_context)
			{
				udev_unref(_context);
			}
		}

		operator udev* () const
		{
			return _context;
		}

		udev_enumerate* CreateEnumerator()
		{
			udev_enumerate* enumerator = udev_enumerate_new(_context);

			if (enumerator)
			{
				return _enumerators.emplace_back(enumerator);
			}

			std::cerr << "Could not create enumerator!" << std::endl;
			return nullptr;
		}

	private:
		udev* _context = udev_new();
		std::vector<udev_enumerate*> _enumerators;
	};

	class UDevice
	{
	public:
		UDevice(UDeviceContext& context, udev_list_entry* device_list_entry) :
			_context(context)
		{
			const char* path = udev_list_entry_get_name(device_list_entry);

			if (!path)
			{
				std::cerr << "Failed to get device path!" << std::endl;
				return;
			}

			_devicePath = path;
			m_device = udev_device_new_from_syspath(context, path);

			if (!_device)
			{
				std::cerr << "Failed to get device from path: '"
					<< _devicePath << "'!" << std::endl;
				return;
			}
		}

		~UDevice()
		{
			if (_device)
			{
				udev_device_unref(_device);
			}
		}

		operator udev_device* () const
		{
			return _device;
		}

		std::string DevicePath() const
		{
			return _devicePath;
		}

		std::string DeviceType() const
		{
			const char* type = udev_device_get_devtype(_device);

			if (!type)
			{
				std::cerr << "Failed to get device type from path: '"
					<< _devicePath << "'!" << std::endl;
				return {};
			}

			return type;
		}

		std::string SystemName() const
		{
			const char* systemName = udev_device_get_sysname(m_device);

			if (!systeName)
			{
				std::cerr << "Failed to get system name from path: '"
					<< _devicePath << "'!" << std::endl;
				return {};
			}

			return systeName;
		}

		udev_list_entry* Partitions()
		{
			udev_enumerate* enumerator = _context.CreateEnumerator();
			udev_enumerate_add_match_parent(enumerator, _device);
			udev_enumerate_add_match_property(enumerator, "DEVTYPE", "partition");
			udev_enumerate_scan_devices(enumerator);
			return udev_enumerate_get_list_entry(enumerator);
		}

		bool IsLoopDevice() const
		{
			return SystemName().compare(0, 4, "loop") == 0;
		}

	private:
		UDeviceContext& _context;
		udev_device* _device = nullptr;
		std::string _devicePath;
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
