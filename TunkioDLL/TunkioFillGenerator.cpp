#include "PCH.hpp"
#include "TunkioFillGenerator.hpp"

#undef max

namespace Tunkio
{
	// Hack to speed up random number generation
	union UInt64Union
	{
		uint64_t u64;
		uint8_t u8[8];
	};

	void Random(std::vector<uint8_t>& data)
	{
		thread_local std::random_device device;
		thread_local std::default_random_engine engine(device());
		thread_local std::uniform_int_distribution<uint64_t> distribution(0, std::numeric_limits<uint64_t>::max());

		UInt64Union randomNumber;

		for (size_t i = 0; i + 8 < data.size(); i += 8)
		{
			randomNumber.u64 = distribution(engine);
			data[i + 0] = randomNumber.u8[0];
			data[i + 1] = randomNumber.u8[1];
			data[i + 2] = randomNumber.u8[2];
			data[i + 3] = randomNumber.u8[3];
			data[i + 4] = randomNumber.u8[4];
			data[i + 5] = randomNumber.u8[5];
			data[i + 6] = randomNumber.u8[6];
			data[i + 7] = randomNumber.u8[7];
		}
	}

	FillGenerator::FillGenerator(const TunkioFillMode mode, const Tunkio::DataUnit::Mebibyte size) :
		m_mode(mode)
	{
		switch (mode)
		{
			case TunkioFillMode::Zeroes:
				m_data.resize(size.Bytes(), 0u);
				break;
			case TunkioFillMode::Ones:
				m_data.resize(size.Bytes(), 0xFFu);
				break;
			case TunkioFillMode::Random:

				if (size.Bytes() % 8 != 0)
				{
					std::cerr << "Warning, requested size not divisible by 8. A few last bytes might not get randomized." << std::endl;
				}

				m_data.resize(size.Bytes());
				break;
		}
	}

	const uint8_t* FillGenerator::Data()
	{
		switch (m_mode)
		{
			case TunkioFillMode::Ones:
			case TunkioFillMode::Zeroes:
				break;
			case TunkioFillMode::Random:
				Random(m_data);
				break;
		}

		return m_data.data();
	}
}