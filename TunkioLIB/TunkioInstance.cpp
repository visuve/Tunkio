#include "TunkioLIB-PCH.hpp"
#include "TunkioInstance.hpp"

namespace Tunkio
{
	Instance::Instance(void* context, const char* path, TunkioTargetType type) :
		m_handle(TunkioInitialize(context, path, type))
	{
	}

	Instance::~Instance()
	{
		if (m_handle)
		{
			TunkioFree(m_handle);
		}
	}

	TunkioHandle* Instance::Get() const
	{
		return m_handle;
	}

	Instance::operator TunkioHandle* ()
	{
		return m_handle;
	}
}
