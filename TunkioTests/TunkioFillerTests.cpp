#include "TunkioTests-PCH.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioSentenceFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"
#include "FillProviders/TunkioFileFiller.hpp"

namespace Tunkio::Fill
{
	TEST(TunkioFillTest, CharFiller)
	{
		CharFiller filler(u8'X', false);
		auto data = filler.Data(3);
		EXPECT_EQ(memcmp(data, u8"XXX", 3), 0);
	}

	TEST(TunkioFillTest, SentenceFiller)
	{
		{
			SentenceFiller filler("foobar\n", false);
			const void* data = filler.Data(3);
			EXPECT_EQ(memcmp(data, "foo", 3), 0);
		}
		{
			SentenceFiller filler("foobar\n", false);
			const void* data = filler.Data(14);
			EXPECT_EQ(memcmp(data, "foobar\nfoobar\n", 14), 0);
		}
		{
			SentenceFiller filler("foobar\n", false);
			const void* data = filler.Data(15);
			EXPECT_EQ(memcmp(data, "foobar\nfoobar\nf", 15), 0);
		}
	}

	TEST(TunkioFillTest, FileFiller)
	{
		const char8_t* str = u8"foobar\n";
		std::vector<char8_t> fileContent = { str, str + 7 };

		{
			FileFiller filler(fileContent, false);
			const auto data = reinterpret_cast<const char8_t*>(filler.Data(3));
			EXPECT_EQ(memcmp(data, u8"foo", 3), 0);
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = reinterpret_cast<const char8_t*>(filler.Data(14));
			EXPECT_EQ(memcmp(data, u8"foobar\nfoobar\n", 14), 0);
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = reinterpret_cast<const char8_t*>(filler.Data(15));
			EXPECT_EQ(memcmp(data, u8"foobar\nfoobar\nf", 15), 0);
		}
	}

	TEST(TunkioFillTest, RandomFillByte)
	{
		RandomFiller filler(false);
		auto data = filler.Data(1);
		EXPECT_NE(data, nullptr);
	}

	TEST(TunkioFillTest, RandomFillNotDivisibleByEight)
	{
		RandomFiller filler(false);

		auto data = reinterpret_cast<const char8_t*>(filler.Data(10));
		std::vector<char8_t> data1 = { data, data + 10 };

		data = reinterpret_cast<const char8_t*>(filler.Data(10));
		std::vector<char8_t> data2 = { data, data + 10 };

		EXPECT_NE(memcmp(data1.data(), data2.data(), 10), 0);

		// This might hold true, as the data is not divisible by 8
		//EXPECT_TRUE(data1[8] == data2[8]);
		//EXPECT_TRUE(data1[9] == data2[9]);
	}
}
