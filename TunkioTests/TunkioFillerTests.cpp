#include "PCH.hpp"
#include "TunkioDataUnits.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioStringFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"

namespace Tunkio::Fill
{
	constexpr Tunkio::DataUnit::Kibibytes Kibibyte(1);

	TEST(TunkioFillTest, ZeroFill)
	{
		CharFiller filler(0xAB, Kibibyte);

		uint8_t* data = filler.Data();

		for (size_t i = 0; i < Kibibyte.Bytes(); ++i)
		{
			EXPECT_EQ(data[i], 0xAB);
		}
	}

	TEST(TunkioFillTest, StringFill)
	{
		StringFiller filler("foobar", 14);

		uint8_t* data = filler.Data();
		uint8_t* iter = data;

		for (uint8_t c : "foobar\0foobar")
		{
			EXPECT_EQ(c, *iter++);
		}
	}

	TEST(TunkioFillTest, RandomFill)
	{
		RandomFiller filler(Kibibyte);
		auto data = filler.Data();
		EXPECT_NE(data, nullptr);
	}

	TEST(TunkioFillTest, RandomFillIllustrateProblem)
	{
		RandomFiller filler(DataUnit::Bytes(10));

		auto data = filler.Data();
		EXPECT_NE(data, nullptr);
		EXPECT_EQ(data[8], 0); // Last two remain unrandomized, known issue ;<
		EXPECT_EQ(data[9], 0);
	}
}