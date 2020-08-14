#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioDeviceWipe.hpp"
#include "../TunkioFillStrategy.hpp"
#include "TunkioWin32AutoHandle.hpp"

namespace Tunkio
{
	// https://docs.microsoft.com/en-us/windows/win32/fileio/file-buffering?redirectedfrom=MSDN
	constexpr uint32_t DesiredAccess = GENERIC_READ | GENERIC_WRITE;
	constexpr uint32_t ShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	constexpr uint32_t CreationFlags = FILE_FLAG_SEQUENTIAL_SCAN | FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH;

	class DeviceWipeImpl : IOperation
	{
	public:

		DeviceWipeImpl(const TunkioOptions* options) :
			IOperation(options)
		{
		}

		~DeviceWipeImpl()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(GetLastError());
				return false;
			}

			if (!m_size)
			{
				ReportError(ErrorCode::NoData);
				return false;
			}

			return Fill();
		}

		bool Open() override
		{
			const std::string path(m_options->Path.Data, m_options->Path.Length);
			m_handle.Reset(
				CreateFileA(path.c_str(), DesiredAccess, ShareMode, nullptr, OPEN_EXISTING, CreationFlags, nullptr));

			if (!m_handle.IsValid())
			{
				return false;
			}

			unsigned long bytesReturned = 0;
			DISK_GEOMETRY diskGeo = { 0 };
			constexpr uint32_t diskGeoSize = sizeof(DISK_GEOMETRY);

			if (!DeviceIoControl(
				m_handle.Value(), IOCTL_DISK_GET_DRIVE_GEOMETRY, nullptr, 0, &diskGeo, diskGeoSize, &bytesReturned, nullptr))
			{
				return false;
			}

			_ASSERT(bytesReturned == sizeof(DISK_GEOMETRY));

			m_size =
				diskGeo.Cylinders.QuadPart * diskGeo.TracksPerCylinder * diskGeo.SectorsPerTrack * diskGeo.BytesPerSector;

			return true;
		}

		bool Remove() override
		{
			return false;
		}

		void ReportStarted() const
		{
			m_options->Callbacks.StartedCallback(m_size);
		}

		bool ReportProgress() const
		{
			return m_options->Callbacks.ProgressCallback(m_totalBytesWritten);
		}

		void ReportError(uint32_t error) const
		{
			m_options->Callbacks.ErrorCallback(error, m_totalBytesWritten);
		}

		void ReportComplete() const
		{
			m_options->Callbacks.CompletedCallback(m_totalBytesWritten);
		}

		bool Fill() override
		{
			DWORD bytesWritten = 0u;
			uint64_t bytesLeft = m_size;
			FillStrategy fakeData(m_options->Mode, DataUnit::Mebibyte(1));

			ReportStarted();

			while (bytesLeft)
			{
				if (fakeData.Size<uint64_t>() > bytesLeft)
				{
					fakeData.Resize(bytesLeft);
				}

				const bool result =
					WriteFile(m_handle.Value(), fakeData.Front(), fakeData.Size<uint32_t>(), &bytesWritten, nullptr);

				m_totalBytesWritten += bytesWritten;
				bytesLeft -= bytesWritten;

				if (!result)
				{
					ReportError(GetLastError());
					return false;
				}

				if (!ReportProgress())
				{
					return true;
				}
			}

			ReportComplete();
			return true;
		}

	private:
		uint64_t m_size = 0;
		uint64_t m_totalBytesWritten = 0;
		Win32AutoHandle m_handle;
	};

	DeviceWipe::DeviceWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new DeviceWipeImpl(options))
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

	bool DeviceWipe::Fill()
	{
		return m_impl->Fill();
	}

	bool DeviceWipe::Remove()
	{
		return m_impl->Remove();
	}
}