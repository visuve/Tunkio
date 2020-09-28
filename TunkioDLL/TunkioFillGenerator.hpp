#pragma once

#include "TunkioAPI.h"
#include "TunkioDataUnits.hpp"

namespace Tunkio
{
	class FillGenerator
	{
	public:
		FillGenerator(const TunkioFillMode mode, const Tunkio::DataUnit::Mebibyte size);

		const uint8_t* Data();

	private:
		const TunkioFillMode m_mode;
		std::vector<uint8_t> m_data;
	};
}