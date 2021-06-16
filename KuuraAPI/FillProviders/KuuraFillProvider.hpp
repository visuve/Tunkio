#pragma once

#include <cstdlib>

namespace Kuura
{
	class IFillProvider
	{
	public:
		explicit IFillProvider(bool verify) :
			Verify(verify)
		{
		}

		virtual ~IFillProvider()
		{
			_fillData.clear();
		}

		virtual std::span<std::byte> Data(uint64_t bytes) = 0;

		const bool Verify;

	protected:
		std::vector<std::byte> _fillData;
	};
}
