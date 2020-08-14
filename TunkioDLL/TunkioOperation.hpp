#pragma once

#include "TunkioAPI.h"

namespace Tunkio
{
	class IOperation
	{
	public:
		IOperation(const TunkioOptions* options) :
			m_options(options)
		{
		}

		virtual ~IOperation() = default;
		virtual bool Run() = 0;
		virtual bool Remove() = 0;
		virtual bool Open() = 0;
		virtual uint64_t Size() = 0;
		virtual bool Fill() = 0;

	protected:
		const TunkioOptions* m_options;
	};
}