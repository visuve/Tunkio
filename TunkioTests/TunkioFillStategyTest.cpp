#include "PCH.hpp"
#include "TunkioFillGenerator.hpp"
#include "TunkioDataUnits.hpp"

namespace Tunkio::Fill
{
	constexpr Tunkio::DataUnit::Mebibyte Mebibyte(1);

	TEST(TunkioFillTest, ZeroFill)
	{
		FillGenerator fill(TunkioMode::Zeroes, 1);

		auto data = fill.Data();

		for (size_t i = 0; i < Mebibyte.Bytes(); ++i)
		{
			EXPECT_EQ(data[i], 0);
		}
	}

	TEST(TunkioFillTest, OneFill)
	{
		FillGenerator fill(TunkioMode::Ones, Mebibyte);

		auto data = fill.Data();

		for (size_t i = 0; i < Mebibyte.Bytes(); ++i)
		{
			EXPECT_EQ(data[i], 1);
		}
	}

	TEST(TunkioFillTest, RandomFill)
	{
		FillGenerator fill(TunkioMode::Random, Mebibyte);

		auto data = fill.Data();
		EXPECT_NE(data, nullptr);
	}

	TEST(TunkioFillTest, RandomFillIllustrateProblem)
	{
		FillGenerator fill(TunkioMode::Random, DataUnit::Byte(10));

		auto data = fill.Data();
		EXPECT_NE(data, nullptr);
		EXPECT_EQ(data[8], 0); // Last two remain unrandomized, known issue ;<
		EXPECT_EQ(data[9], 0);
	}
}