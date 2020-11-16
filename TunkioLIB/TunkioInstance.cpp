#include "TunkioLIB-PCH.hpp"
#include "TunkioInstance.hpp"

namespace Tunkio
{
	Instance::Instance() :
		AutoHandle()
	{
	}

	Instance::Instance(void* context, const char* path, TunkioTargetType type) :
		AutoHandle(TunkioInitialize(context, path, type))
	{
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
