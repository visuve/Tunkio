#include "../KuuraAPI-PCH.hpp"
#include "KuuraSequenceFiller.hpp"

namespace Kuura
{
	SequenceFiller::SequenceFiller(
		const std::string& fillString,
		bool verify) :
		IFillProvider(verify),
		_fillString(fillString)
	{
	}

	std::span<std::byte> SequenceFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		if (_fillData.size() != bytes)
		{
			_fillData.resize(bytes);
			AlignData(alignment);
		}

		for (size_t i = 0; i < bytes; ++i)
		{
			_fillData[i] = static_cast<std::byte>(_fillString[_offset++]);

			if (_offset >= _fillString.size())
			{
				_offset = 0;
			}
		}

		return _fillData;
	}
}
