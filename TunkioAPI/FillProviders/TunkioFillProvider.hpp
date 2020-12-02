#pragma once

namespace Tunkio
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
			m_fillData.clear();
		}

		virtual std::byte* Data(uint64_t bytes) = 0;

		const bool Verify;

	protected:
		std::vector<std::byte> m_fillData;
	};
}
