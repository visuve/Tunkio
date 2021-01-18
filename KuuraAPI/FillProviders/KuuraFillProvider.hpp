#pragma once

#include <cstdlib>

#if WIN32
#define UNUSED_IN_RELEASE_MODE
#else
#define UNUSED_IN_RELEASE_MODE __attribute__((unused))
#endif

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
			m_fillData.clear();
		}

		virtual std::span<std::byte> Data(uint64_t bytes, uint64_t alignment) = 0;

		inline void AlignData(uint64_t alignment)
		{
			if (alignment)
			{
				size_t space = m_fillData.size();
				void* data = m_fillData.data();

				assert(space && space % 512 == 0);
				assert(space >= alignment);

				void* UNUSED_IN_RELEASE_MODE ptr = std::align(
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
