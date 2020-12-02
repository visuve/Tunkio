#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class SequenceFiller : public IFillProvider
	{
	public:
		explicit SequenceFiller(const std::string& fillString, bool verify);
		~SequenceFiller() = default;

		std::byte* Data(uint64_t bytes) override;

	protected:
		const std::string m_fillString;

	private:
		size_t m_offset = 0;
	};
}
