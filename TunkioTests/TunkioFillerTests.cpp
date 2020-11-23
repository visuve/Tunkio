#include "TunkioTests-PCH.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioSentenceFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"

namespace Tunkio::Fill
{
	constexpr Tunkio::DataUnit::Kibibytes Kibibyte(1);

	TEST(TunkioFillTest, ZeroFill)
	{
		CharFiller filler(Kibibyte, 0xAB, false);

		auto data = reinterpret_cast<const uint8_t*>(filler.Data());

		for (size_t i = 0; i < Kibibyte.Bytes(); ++i)
		{
			EXPECT_EQ(data[i], 0xAB);
		}
	}

	TEST(TunkioFillTest, StringFill)
	{
		{
			SentenceFiller filler(13, "foobar", false);

			auto data = reinterpret_cast<const char*>(filler.Data());
			size_t iter = 0;

			for (char c : "foobar\0foobar")
			{
				EXPECT_EQ(c, data[iter++]);
			}
		}
		{
			SentenceFiller filler(14, "foobar\r\n", false);

			auto data = reinterpret_cast<const char*>(filler.Data());
			size_t iter = 0;

			for (char c : "foobar\r\nfoobar")
			{
				EXPECT_EQ(c, data[iter++]);
			}

			EXPECT_EQ(14, strlen(data));
		}
		{
			SentenceFiller filler(26, "foobar\tbarfoo\n", false);

			auto data = reinterpret_cast<const char*>(filler.Data());
			size_t iter = 0;

			for (char c : "foobar\tbarfoo\nfoobar\tbarfoo")
			{
				EXPECT_EQ(c, data[iter++]);
			}

			EXPECT_EQ(27, strlen(data));
		}
	}

	TEST(TunkioFillTest, RandomFill)
	{
		RandomFiller filler(Kibibyte, false);
		auto data = filler.Data();
		EXPECT_NE(data, nullptr);
	}

	TEST(TunkioFillTest, RandomFillIllustrateProblem)
	{
		RandomFiller filler(DataUnit::Bytes(10), false);

		auto data = reinterpret_cast<const uint8_t*>(filler.Data());
		EXPECT_NE(data, nullptr);
		EXPECT_EQ(data[8], 0); // Last two remain unrandomized, known issue ;<
		EXPECT_EQ(data[9], 0);
	}
}
