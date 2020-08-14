#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioDeviceWipe.hpp"
#include "TunkioWin32Wipe.hpp"

namespace Tunkio
{
	class Win32DeviceWipe : public Win32Wipe
	{
	public:
		Win32DeviceWipe(const TunkioOptions* options) :
			Win32Wipe(options)
		{
		}

		~Win32DeviceWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(GetLastError());
				return false;
			}

			m_size = Size();

			if (!m_size)
			{
				ReportError(ErrorCode::NoData);
				return false;
			}

			return Win32Wipe::Fill();
		}

		uint64_t Size() override
		{
			if (!m_handle.IsValid())
			{
				return 0;
			}

			unsigned long bytesReturned = 0;
			DISK_GEOMETRY diskGeo = { 0 };
			constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

			if (!DeviceIoControl(m_handle.Value(),
				IOCTL_DISK_GET_DRIVE_GEOMETRY,
				nullptr,
				0,
				&diskGeo,
				diskGeoSize,
				&bytesReturned,
				nullptr))
			{
				return false;
			}

			_ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));

			return diskGeo.Cylinders.QuadPart *
				diskGeo.TracksPerCylinder *
				diskGeo.SectorsPerTrack *
				diskGeo.BytesPerSector;
		}

		bool Remove() override
		{
			return false;
		}
	};

	DeviceWipe::DeviceWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new Win32DeviceWipe(options))
	{
	}

	DeviceWipe::~DeviceWipe()
	{
		delete m_impl;
	}

	bool DeviceWipe::Run()
	{
		return m_impl->Run();
	}

	bool DeviceWipe::Open()
	{
		return m_impl->Open();
	}

	uint64_t DeviceWipe::Size()
	{
		return m_impl->Size();
	}

	bool DeviceWipe::Fill()
	{
		return m_impl->Fill();
	}

	bool DeviceWipe::Remove()
	{
		return m_impl->Remove();
	}
}