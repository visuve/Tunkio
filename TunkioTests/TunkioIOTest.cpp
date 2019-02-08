#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioIO.hpp"

namespace Tunkio
{
    TEST(TunkioTest, DiskSizeSuccess)
    {
        EXPECT_EQ(IO::DiskSize(nullptr), 210);
    }

    TEST(TunkioTest, WipeDiskSuccess)
    {
        IO::Mock::SetFakeWrite([](LPDWORD bytesWritten)
        {
            *bytesWritten = 0x400;
            return true;
        });

        uint64_t bytesLeft = 0x1000;
        uint64_t bytesWritten = 0;

        EXPECT_TRUE(IO::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
        EXPECT_EQ(bytesLeft, 0);
        EXPECT_EQ(bytesWritten, 0x1000);
    }

    TEST(TunkioTest, WipeDiskImmediateFail)
    {
        IO::Mock::SetFakeWrite([](LPDWORD)
        {
            return false;
        });

        uint64_t bytesLeft = 0x1000;
        uint64_t bytesWritten = 0;

        EXPECT_FALSE(IO::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
        EXPECT_EQ(bytesLeft, 0x1000);
        EXPECT_EQ(bytesWritten, 0);
    }

    TEST(TunkioTest, WipeDiskPartialFail)
    {
        int calls = 0;
        IO::Mock::SetFakeWrite([&](LPDWORD bytesWritten)
        {
            *bytesWritten = 0x400;
            return ++calls < 2;
        });

        uint64_t bytesLeft = 0x1000;
        uint64_t bytesWritten = 0;

        EXPECT_FALSE(IO::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
        EXPECT_EQ(bytesLeft, 0x800);
        EXPECT_EQ(bytesWritten, 0x800);
    }
}