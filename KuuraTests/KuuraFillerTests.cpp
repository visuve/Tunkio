#include "KuuraTests-PCH.hpp"

#include "FillProviders/KuuraFillProvider.hpp"
#include "FillProviders/KuuraByteFiller.hpp"
#include "FillProviders/KuuraSequenceFiller.hpp"
#include "FillProviders/KuuraRandomFiller.hpp"
#include "FillProviders/KuuraFileFiller.hpp"

namespace Kuura::Fill
{
	bool operator == (std::span<std::byte> lhs, std::string_view rhs)
	{
		if (lhs.size() != rhs.size())
		{
			return false;
		}

		return memcmp(lhs.data(), rhs.data(), rhs.size()) == 0;
	}

	TEST(KuuraFillTest, ByteFiller)
	{
		ByteFiller filler(std::byte(0x58), false);
		auto data = filler.Data(3, 0);
		EXPECT_TRUE(data == "XXX");
	}

	TEST(KuuraFillTest, SequenceFiller)
	{
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(3, 0);
			EXPECT_TRUE(data == "foo");
		}
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(14, 0);
			EXPECT_TRUE(data == "foobar\nfoobar\n");
		}
		{
			SequenceFiller filler("foobar\n", false);
			const auto data = filler.Data(15, 0);
			EXPECT_TRUE(data == "foobar\nfoobar\nf");
		}
		{
			SequenceFiller filler("foobar", false);
			{
				const auto data = filler.Data(3, 0);
				EXPECT_TRUE(data == "foo");
			}
			{
				const auto data = filler.Data(3, 0);
				EXPECT_TRUE(data == "bar");
			}
			{
				const auto data = filler.Data(3, 0);
				EXPECT_TRUE(data == "foo");
			}
		}
		{
			SequenceFiller filler("foobar", false);
			{
				const auto data = filler.Data(3, 0);
				EXPECT_TRUE(data == "foo");
			}
			{
				const auto data = filler.Data(4, 0);
				EXPECT_TRUE(data == "barf");
			}
			{
				const auto data = filler.Data(2, 0);
				EXPECT_TRUE(data == "oo");
			}
		}
	}

	TEST(KuuraFillTest, FileFiller)
	{
		const auto str = reinterpret_cast<const std::byte*>(u8"foobar\n");

		std::vector<std::byte> fileContent = { str, str + 7 };

		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(3, 0);
			EXPECT_TRUE(data == "foo");
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(14, 0);
			EXPECT_TRUE(data == "foobar\nfoobar\n");
		}
		{
			FileFiller filler(fileContent, false);
			const auto data = filler.Data(15, 0);
			EXPECT_TRUE(data == "foobar\nfoobar\nf");
		}
	}

	TEST(KuuraFillTest, RandomFillByte)
	{
		RandomFiller filler(false);

		for (uint64_t i = 0; i < 10; ++i)
		{
			EXPECT_NO_THROW(filler.Data(i, 0));
		}
	}

	TEST(KuuraFillTest, RandomFillTwice)
	{
		RandomFiller filler(false);
		std::vector<std::byte> before;
		std::vector<std::byte> after;

		{
			auto data = filler.Data(8, 0);
			before.insert(before.begin(), data.begin(), data.end());
		}
		{
			auto data = filler.Data(8, 0);
			after.insert(after.begin(), data.begin(), data.end());
		}

		EXPECT_NE(before, after);
	}

	TEST(KuuraFillTest, RandomFillDifferentFillers)
	{
		RandomFiller fillerA(false);
		RandomFiller fillerB(false);

		std::vector<std::byte> fillA;
		std::vector<std::byte> fillB;

		{
			auto data = fillerA.Data(8, 0);
			fillA.insert(fillA.begin(), data.begin(), data.end());
		}
		{
			auto data = fillerB.Data(8, 0);
			fillB.insert(fillB.begin(), data.begin(), data.end());
		}

		EXPECT_NE(fillA, fillB);
	}
}
