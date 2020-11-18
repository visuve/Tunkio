#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class CharFiller : public IFillProvider
	{
	public:
		explicit CharFiller(
			DataUnit::Bytes bytes,
			uint8_t byte,
			bool verify);

		virtual ~CharFiller();

		const void* Data() override;

	protected:
		uint8_t m_fillChar;
		std::vector<uint8_t> m_fillData;
	};
}
