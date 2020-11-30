#pragma once

namespace Tunkio
{
	class IFillProvider
	{
	public:
		constexpr explicit IFillProvider(bool verify) :
			Verify(verify)
		{
		}

		virtual ~IFillProvider() = default;

		virtual const void* Data(uint64_t bytes) = 0;

		const bool Verify;
	};
}
