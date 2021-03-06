#include "../KuuraAPI-PCH.hpp"
#include "KuuraByteFiller.hpp"

namespace Kuura
{
	ByteFiller::ByteFiller(std::byte byte, bool verify) :
		IFillProvider(verify),
		_byte(byte)
	{
	}

	std::span<std::byte> ByteFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		if (_fillData.size() != bytes)
		{
			_fillData.resize(bytes, _byte);
			AlignData(alignment);
		}

		return _fillData;
	}
}
