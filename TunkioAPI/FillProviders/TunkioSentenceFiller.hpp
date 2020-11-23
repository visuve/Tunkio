#pragma once

#include "TunkioFillProvider.hpp"

namespace Tunkio
{
	class SentenceFiller : public IFillProvider
	{
	public:
		explicit SentenceFiller(
			DataUnit::Bytes bytes,
			const std::string& fillString,
			bool verify);

		virtual ~SentenceFiller();

		const void* Data() override;

	protected:
		std::string m_fillString;
		std::string m_fillData;
	};
}