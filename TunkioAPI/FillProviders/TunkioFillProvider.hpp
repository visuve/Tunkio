#pragma once

#include <cstdlib>

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

		virtual std::span<std::byte> Data(uint64_t bytes, uint64_t alignment) = 0;

		inline void AlignData(uint64_t bytes, uint64_t alignment)
		{
			assert(bytes);
			assert(bytes % 512 == 0);

			if (alignment)
			{
				size_t space = m_fillData.size();
				void* data = m_fillData.data();

				void* ptr = std::align(
					alignment,
					1,
					data,
					space);

				assert(ptr);
			}
		}

		const bool Verify;

	protected:
		std::vector<std::byte> m_fillData;
	};
}
