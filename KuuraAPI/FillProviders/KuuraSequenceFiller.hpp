#pragma once

#include "KuuraFillProvider.hpp"

namespace Kuura
{
	class SequenceFiller : public IFillProvider
	{
	public:
		explicit SequenceFiller(const std::string& fillString, bool verify);
		~SequenceFiller() = default;

		std::span<std::byte> Data(uint64_t bytes) override;

	protected:
		const std::string _fillString;

	private:
		size_t _offset = 0;
	};
}
