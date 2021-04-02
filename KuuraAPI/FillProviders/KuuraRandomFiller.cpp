#include "../KuuraAPI-PCH.hpp"
#include "KuuraRandomFiller.hpp"

namespace Kuura
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

		if (_fillData.size() != bytes)
		{
			_fillData.resize(bytes);
			AlignData(alignment);

			UInt64Union randomNumber;

			while (bytes >= 8)
			{
				randomNumber.u64 = distribution(engine);
				_fillData[--bytes] = randomNumber.byte[0];
				_fillData[--bytes] = randomNumber.byte[1];
				_fillData[--bytes] = randomNumber.byte[2];
				_fillData[--bytes] = randomNumber.byte[3];
				_fillData[--bytes] = randomNumber.byte[4];
				_fillData[--bytes] = randomNumber.byte[5];
				_fillData[--bytes] = randomNumber.byte[6];
				_fillData[--bytes] = randomNumber.byte[7];
			}
		}
		else
		{
			// This hack almost doubles the performance
			UInt64Union randomNumber = { distribution(engine) };

			while (bytes >= 8)
			{
				_fillData[--bytes] ^= randomNumber.byte[0];
				_fillData[--bytes] ^= randomNumber.byte[1];
				_fillData[--bytes] ^= randomNumber.byte[2];
				_fillData[--bytes] ^= randomNumber.byte[3];
				_fillData[--bytes] ^= randomNumber.byte[4];
				_fillData[--bytes] ^= randomNumber.byte[5];
				_fillData[--bytes] ^= randomNumber.byte[6];
				_fillData[--bytes] ^= randomNumber.byte[7];
			}
		}

		return _fillData;
	}
}
