#pragma once

#include "TunkioAPI.h"
#include "TunkioOperation.hpp"

namespace Tunkio
{
	class DirectoryWipe : public IOperation
	{
	public:
		DirectoryWipe(const TunkioOptions* options);
		~DirectoryWipe();

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