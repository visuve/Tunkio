#pragma once

namespace Tunkio
{
	class IFillProvider
	{
	public:
		constexpr explicit IFillProvider(bool verify) :
			m_verify(verify)
		{
		}

		virtual ~IFillProvider() = default;

		virtual const void* Data(uint64_t bytes) = 0;

	protected:
		bool m_verify;
	};
}
