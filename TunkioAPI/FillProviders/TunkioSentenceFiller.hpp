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

		explicit SentenceFiller(
			DataUnit::Bytes bytes,
			const std::string& fillString,
			const std::string& padding,
			bool verify);

		virtual ~SentenceFiller();

		const void* Data() override;

	protected:
		std::string m_fillString;
		std::string m_padding;
		std::string m_fillData;
	};
}