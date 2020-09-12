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

	protected:
		const TunkioOptions* m_options;
	};
}