#include "../PCH.hpp"
#include "TunkioRandomFiller.hpp"

namespace Tunkio
{
	// Hack to speed up random number generation
	union UInt64Union
	{
		uint64_t u64;
		uint8_t u8[8];
	};

	RandomFiller::RandomFiller(DataUnit::Byte bytes) :
		CharFiller(0x00, bytes)
	{
	}

	RandomFiller::~RandomFiller()
	{
	}

	uint8_t* RandomFiller::Data()
	{
		if (m_fillData.empty())
		{
			m_fillData.resize(m_size.Bytes(), m_fillChar);
		}

		thread_local std::random_device device;
		thread_local std::default_random_engine engine(device());
		thread_local std::uniform_int_distribution<uint64_t>
			distribution(0, std::numeric_limits<uint64_t>::max());

		UInt64Union randomNumber;

		for (size_t i = 0; i + 8 < m_fillData.size();)
		{
			randomNumber.u64 = distribution(engine);
			m_fillData[i++] = randomNumber.u8[0];
			m_fillData[i++] = randomNumber.u8[1];
			m_fillData[i++] = randomNumber.u8[2];
			m_fillData[i++] = randomNumber.u8[3];
			m_fillData[i++] = randomNumber.u8[4];
			m_fillData[i++] = randomNumber.u8[5];
			m_fillData[i++] = randomNumber.u8[6];
			m_fillData[i++] = randomNumber.u8[7];
		}

		return m_fillData.data();
	}
}