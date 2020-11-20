#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class StringFiller : public IFillProvider
	{
	public:
		explicit StringFiller(
			DataUnit::Bytes bytes,
			const std::string& fillString,
			bool verify);

		~StringFiller();

		const void* Data() override;

	private:
		std::string m_fillString;
		std::string m_fillData;
	};
}