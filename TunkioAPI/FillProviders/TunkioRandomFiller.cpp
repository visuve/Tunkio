#include "../TunkioAPI-PCH.hpp"
#include "TunkioRandomFiller.hpp"

namespace Tunkio
{
	// Hack to speed up random number generation
	union UInt64Union
	{
		uint64_t u64;
		std::byte byte[8];
	};

	RandomFiller::RandomFiller(bool verify) :
		IFillProvider(verify)
	{
	}

	std::span<std::byte> RandomFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		thread_local std::random_device device;
		thread_local std::mt19937_64 engine(device());
		thread_local std::uniform_int_distribution<uint64_t> distribution;

		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);
			AlignData(alignment);

			UInt64Union randomNumber;

			while (bytes >= 8)
			{
				randomNumber.u64 = distribution(engine);
				m_fillData[--bytes] = randomNumber.byte[0];
				m_fillData[--bytes] = randomNumber.byte[1];
				m_fillData[--bytes] = randomNumber.byte[2];
				m_fillData[--bytes] = randomNumber.byte[3];
				m_fillData[--bytes] = randomNumber.byte[4];
				m_fillData[--bytes] = randomNumber.byte[5];
				m_fillData[--bytes] = randomNumber.byte[6];
				m_fillData[--bytes] = randomNumber.byte[7];
			}
		}
		else
		{
			// This hack almost doubles the performance
			UInt64Union randomNumber = { distribution(engine) };

			while (bytes >= 8)
			{
				m_fillData[--bytes] ^= randomNumber.byte[0];
				m_fillData[--bytes] ^= randomNumber.byte[1];
				m_fillData[--bytes] ^= randomNumber.byte[2];
				m_fillData[--bytes] ^= randomNumber.byte[3];
				m_fillData[--bytes] ^= randomNumber.byte[4];
				m_fillData[--bytes] ^= randomNumber.byte[5];
				m_fillData[--bytes] ^= randomNumber.byte[6];
				m_fillData[--bytes] ^= randomNumber.byte[7];
			}
		}

		return m_fillData;
	}
}
