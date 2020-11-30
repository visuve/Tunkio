#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class SentenceFiller : public IFillProvider
	{
	public:
		explicit SentenceFiller(const std::string& fillString, bool verify);
		virtual ~SentenceFiller();

		const void* Data(uint64_t bytes) override;

	protected:
		const std::string m_fillString;
		std::string m_fillData;

	private:
		size_t m_offset = 0;
	};
}
