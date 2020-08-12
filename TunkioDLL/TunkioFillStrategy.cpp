#include "PCH.hpp"
#include "TunkioFillStrategy.hpp"

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

	FillStrategy::FillStrategy(const TunkioMode mode, const Tunkio::DataUnit::Mebibyte size) :
		m_mode(mode)
	{
		switch (mode)
		{
			case TunkioMode::Zeroes:
				m_data = std::vector<uint8_t>(static_cast<size_t>(size.Bytes()), 0u);
				break;
			case TunkioMode::Ones:
				m_data = std::vector<uint8_t>(static_cast<size_t>(size.Bytes()), 1u);
				break;
			case TunkioMode::Random:
				if (size.Bytes() % 8 != 0)
				{
					std::cerr << "Warning, requested size not divisible by 8. A few last bytes might not get randomized." << std::endl;
				}
				m_data = std::vector<uint8_t>(static_cast<size_t>(size.Bytes()));
				Random(m_data);
				break;
		}
	}

	const uint8_t* FillStrategy::Front()
	{
		switch (m_mode)
		{
			case TunkioMode::Ones:
			case TunkioMode::Zeroes:
				break;
			case TunkioMode::Random:
				Random(m_data);
				break;
		}

		return m_data.data();
	}
}