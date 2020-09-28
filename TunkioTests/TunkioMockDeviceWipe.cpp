#include "PCH.hpp"
#include "TunkioDeviceWipe.hpp"

namespace Tunkio
{
	DeviceWipe::DeviceWipe(const std::string& path) :
		IOperation(path)
	{
	}

	bool DeviceWipe::Run()
	{
		m_startedCallback(1);
		m_progressCallback(1);
		m_completedCallback(1);
		return true;
	}
}