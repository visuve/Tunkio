#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class RandomFiller : public IFillProvider
	{
	public:
		explicit RandomFiller(bool verify);
		~RandomFiller();

		const void* Data(uint64_t bytes) override;

	private:
		std::vector<char8_t> m_fillData;
	};
}