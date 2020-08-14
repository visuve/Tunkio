#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DeviceWipe : public IOperation
	{
	public:
		DeviceWipe(const TunkioOptions* options);
		~DeviceWipe();

		bool Run() override;
	protected:
		bool Open() override;
		uint64_t Size() override;
		bool Fill() override;
		bool Remove() override;
	private:
		IOperation* m_impl = nullptr;
	};
}