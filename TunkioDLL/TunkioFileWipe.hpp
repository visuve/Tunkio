#pragma once

#include "TunkioAPI.h"
#include "ITunkioOperation.hpp"

namespace Tunkio
{
	class FileWipeImpl;

	class FileWipe : public IOperation
	{
	public:
		FileWipe(const TunkioOptions* options);
		~FileWipe();

		bool Run() override;
	protected:
		bool Open() override;
		bool Fill() override;
		bool Remove() override;
	private:
		FileWipeImpl* m_impl = nullptr;
	};
}