#include "../PCH.hpp"
#include "../TunkioErrorCodes.hpp"
#include "../TunkioDeviceWipe.hpp"
#include "TunkioPosixWipe.hpp"

namespace Tunkio
{
	class PosixDeviceWipe: public PosixWipe
	{
	public:
		PosixDeviceWipe(const TunkioOptions* options) :
			PosixWipe(options)
		{
		}

		~PosixDeviceWipe()
		{
		}

		bool Run() override
		{
			if (!Open())
			{
				ReportError(errno);
				return false;
			}

			m_size = Size();

			if (!m_size)
			{
				ReportError(m_error);
				return false;
			}

			return Fill();
		}

		uint64_t Size() override
		{
			if (!m_handle.IsValid())
			{
				return 0;
			}

			uint64_t size = 0;
			m_error = ioctl(m_handle.Value(), BLKGETSIZE64, &size);

			if (m_error != 0)
			{
				return 0;
			}

			return size;
		}

		bool Remove() override
		{
			return false;
		}

	private:
		int m_error = 0;
	};

	DeviceWipe::DeviceWipe(const TunkioOptions* options) :
		IOperation(options),
		m_impl(new PosixDeviceWipe(options))
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