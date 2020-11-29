#include "../TunkioAPI-PCH.hpp"
#include "TunkioRandomFiller.hpp"

namespace Tunkio
{
	// Hack to speed up random number generation
	union UInt64Union
	{
		uint64_t u64;
		char8_t u8[8];
	};

	RandomFiller::RandomFiller(bool verify) :
		IFillProvider(verify)
	{
	}

	RandomFiller::~RandomFiller()
	{
	}

	const void* RandomFiller::Data(uint64_t bytes)
	{
		thread_local std::random_device device;
		thread_local std::mt19937_64 engine(device());
		thread_local std::uniform_int_distribution<uint64_t> distribution;

		if (m_fillData.size() != bytes)
		{
			m_fillData.resize(bytes);

			UInt64Union randomNumber;

			while (bytes > 8)
			{
				randomNumber.u64 = distribution(engine);
				m_fillData[--bytes] = randomNumber.u8[0];
				m_fillData[--bytes] = randomNumber.u8[1];
				m_fillData[--bytes] = randomNumber.u8[2];
				m_fillData[--bytes] = randomNumber.u8[3];
				m_fillData[--bytes] = randomNumber.u8[4];
				m_fillData[--bytes] = randomNumber.u8[5];
				m_fillData[--bytes] = randomNumber.u8[6];
				m_fillData[--bytes] = randomNumber.u8[7];
			}
		}
		else
		{
			// This hack almost doubles the performance
			UInt64Union randomNumber = { distribution(engine) };

			while (bytes > 8)
			{
				m_fillData[--bytes] ^= randomNumber.u8[0];
				m_fillData[--bytes] ^= randomNumber.u8[1];
				m_fillData[--bytes] ^= randomNumber.u8[2];
				m_fillData[--bytes] ^= randomNumber.u8[3];
				m_fillData[--bytes] ^= randomNumber.u8[4];
				m_fillData[--bytes] ^= randomNumber.u8[5];
				m_fillData[--bytes] ^= randomNumber.u8[6];
				m_fillData[--bytes] ^= randomNumber.u8[7];
			}
		}

		return m_fillData.data();
	}
}
