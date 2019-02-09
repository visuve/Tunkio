#include "PCH.hpp"
#include "TunkioIOMock.hpp"
#include "TunkioIO.hpp"

namespace Tunkio
{
    TEST(TunkioIOTest, FileSizeSuccess)
    {
        EXPECT_EQ(IO::FileSize(nullptr), 123);
    }

    TEST(TunkioIOTest, VolumeSizeSuccess)
    {
        EXPECT_EQ(IO::VolumeSize(nullptr), 210);
    }

    TEST(TunkioIOTest, WipeSuccess)
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

    TEST(TunkioIOTest, WipeImmediateFail)
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

    TEST(TunkioIOTest, WipePartialFail)
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