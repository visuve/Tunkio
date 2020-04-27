#include "PCH.hpp"
#include "Strategies/TunkioFillStrategy.hpp"

namespace Tunkio::Fill
{
    constexpr size_t Megabyte = 1 * 1024 * 1024;
    TEST(TunkioFillTest, Zeroes)
    {
        FillStrategy fill(TunkioMode::Zeroes, Megabyte);

        EXPECT_EQ(fill.Size<size_t>(), Megabyte);

        auto data = fill.Front();

        for (size_t i = 0; i < Megabyte; ++i)
        {
            EXPECT_EQ(data[i], 0);
        }
    }

    TEST(TunkioFillTest, Ones)
    {
        FillStrategy fill(TunkioMode::Ones, Megabyte);

        EXPECT_EQ(fill.Size<size_t>(), Megabyte);

        auto data = fill.Front();

        for (size_t i = 0; i < Megabyte; ++i)
        {
            EXPECT_EQ(data[i], 1);
        }
    }

    TEST(TunkioFillTest, Random)
    {
        FillStrategy fill(TunkioMode::Random, Megabyte);

        EXPECT_EQ(fill.Size<size_t>(), Megabyte);

        auto data = fill.Front();
        EXPECT_NE(data, nullptr);
    }
}