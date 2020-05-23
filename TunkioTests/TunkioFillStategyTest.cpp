#include "PCH.hpp"
#include "Strategies/TunkioFillStrategy.hpp"
#include "TunkioDataUnits.hpp"

namespace Tunkio::Fill
{
    constexpr Tunkio::DataUnit::Mebibyte Mebibyte(1);
    TEST(TunkioFillTest, ZeroFill)
    {
        FillStrategy fill(TunkioMode::Zeroes, 1);

        EXPECT_EQ(fill.Size<size_t>(), Mebibyte.Bytes());

        auto data = fill.Front();

        for (size_t i = 0; i < Mebibyte.Bytes(); ++i)
        {
            EXPECT_EQ(data[i], 0);
        }
    }

    TEST(TunkioFillTest, OneFill)
    {
        FillStrategy fill(TunkioMode::Ones, Mebibyte);

        EXPECT_EQ(fill.Size<size_t>(), Mebibyte.Bytes());

        auto data = fill.Front();

        for (size_t i = 0; i < Mebibyte.Bytes(); ++i)
        {
            EXPECT_EQ(data[i], 1);
        }
    }

    TEST(TunkioFillTest, RandomFill)
    {
        FillStrategy fill(TunkioMode::Random, Mebibyte);

        EXPECT_EQ(fill.Size<size_t>(), Mebibyte.Bytes());

        auto data = fill.Front();
        EXPECT_NE(data, nullptr);
    }

    TEST(TunkioFillTest, RandomFillASD)
    {
        FillStrategy fill(TunkioMode::Random, DataUnit::Byte(10));
        EXPECT_EQ(fill.Size<size_t>(), DataUnit::Byte(10).Bytes());

        auto data = fill.Front();
        EXPECT_NE(data, nullptr);
        EXPECT_EQ(data[8], 0); // Last two remain unrandomized, known issue ;<
        EXPECT_EQ(data[9], 0);
    }
}