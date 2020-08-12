#include "../PCH.hpp"
#include "TunkioPosixAutoHandle.hpp"

namespace Tunkio
{
	PosixAutoHandle::PosixAutoHandle(int handle) :
		m_handle(handle)
	{
	}

	PosixAutoHandle::~PosixAutoHandle()
	{
		Reset();
	}

	void PosixAutoHandle::Reset(int handle)
	{
		if (m_handle == handle)
		{
			return;
		}

		if (m_handle)
		{
			close(m_handle);
		}

		m_handle = handle;
	}

	const int PosixAutoHandle::Descriptor() const
	{
		return m_handle;
	}

	bool PosixAutoHandle::IsValid() const
	{
		return m_handle > 0;
	}
}