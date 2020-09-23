#include "PCH.hpp"
#include "TunkioDeviceWipe.hpp"

namespace Tunkio
{
	DeviceWipe::DeviceWipe(const TunkioOptions* options) :
		IOperation(options)
	{
	}

	bool DeviceWipe::Run()
	{
		m_options->Callbacks.StartedCallback(1);
		m_options->Callbacks.ProgressCallback(1);
		m_options->Callbacks.CompletedCallback(1);
		return true;
	}
}