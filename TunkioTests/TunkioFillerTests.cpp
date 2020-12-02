#include "TunkioTests-PCH.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioByteFiller.hpp"
#include "FillProviders/TunkioSequenceFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"
#include "FillProviders/TunkioFileFiller.hpp"

namespace Tunkio::Fill
{
	TEST(TunkioFillTest, ByteFiller)
	{
		ByteFiller filler(std::byte(0x58), false);
		auto data = filler.Data(3);
		EXPECT_EQ(memcmp(data, "XXX", 3), 0);
	}

	TEST(TunkioFillTest, SequenceFiller)
	{
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(3);
			EXPECT_EQ(memcmp(data, "foo", 3), 0);
		}
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(14);
			EXPECT_EQ(memcmp(data, "foobar\nfoobar\n", 14), 0);
		}
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(15);
			EXPECT_EQ(memcmp(data, "foobar\nfoobar\nf", 15), 0);
		}
		{
			SequenceFiller filler("foobar", false);
			{
				const auto data = filler.Data(3);
				EXPECT_EQ(memcmp(data, u8"foo", 3), 0);
			}
			{
				const auto data = filler.Data(3);
				EXPECT_EQ(memcmp(data, u8"bar", 3), 0);
			}
			{
				const auto data = filler.Data(3);
				EXPECT_EQ(memcmp(data, u8"foo", 3), 0);
			}
		}
		{
			SequenceFiller filler("foobar", false);
			{
				const void* data = filler.Data(3);
				EXPECT_EQ(memcmp(data, u8"foo", 3), 0);
			}
			{
				const void* data = filler.Data(4);
				EXPECT_EQ(memcmp(data, u8"barf", 4), 0);
			}
			{
				const void* data = filler.Data(2);
				EXPECT_EQ(memcmp(data, u8"oo", 2), 0);
			}
		}
	}

	TEST(TunkioFillTest, FileFiller)
	{
		const auto str = reinterpret_cast<const std::byte*>(u8"foobar\n");

		std::vector<std::byte> fileContent = { str, str + 7 };

		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(3);
			EXPECT_EQ(memcmp(data, u8"foo", 3), 0);
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(14);
			EXPECT_EQ(memcmp(data, u8"foobar\nfoobar\n", 14), 0);
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(15);
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

		auto data = reinterpret_cast<const std::byte*>(filler.Data(10));
		std::vector<std::byte> data1 = { data, data + 10 };

		data = reinterpret_cast<const std::byte*>(filler.Data(10));
		std::vector<std::byte> data2 = { data, data + 10 };

		EXPECT_NE(memcmp(data1.data(), data2.data(), 10), 0);

		// This might hold true, as the data is not divisible by 8
		//EXPECT_TRUE(data1[8] == data2[8]);
		//EXPECT_TRUE(data1[9] == data2[9]);
	}
}
