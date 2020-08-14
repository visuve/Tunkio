#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class FileWipe : public IOperation
	{
	public:
		FileWipe(const TunkioOptions* options);
		~FileWipe();

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