#include "PCH.hpp"
#include "FileSystemMock.hpp"
#include "../WinDiskWipe/WDW.hpp"

TEST(WdwTest, DiskSizeSuccess)
{
    EXPECT_EQ(WDW::DiskSize(nullptr), 210);
}

TEST(WdwTest, WipeDiskSuccess)
{
    WDW::Mock::SetFakeWrite([](LPDWORD bytesWritten)
    {
        *bytesWritten = 0x400;
        return true;
    });

    uint64_t bytesLeft = 0x1000;
    uint64_t bytesWritten = 0;

    EXPECT_TRUE(WDW::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
    EXPECT_EQ(bytesLeft, 0);
    EXPECT_EQ(bytesWritten, 0x1000);
}

TEST(WdwTest, WipeDiskImmediateFail)
{
    WDW::Mock::SetFakeWrite([](LPDWORD)
    {
        return false;
    });

    uint64_t bytesLeft = 0x1000;
    uint64_t bytesWritten = 0;

    EXPECT_FALSE(WDW::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
    EXPECT_EQ(bytesLeft, 0x1000);
    EXPECT_EQ(bytesWritten, 0);
}

TEST(WdwTest, WipeDiskPartialFail)
{
    int calls = 0;
    WDW::Mock::SetFakeWrite([&](LPDWORD bytesWritten)
    {
        *bytesWritten = 0x400;
        return ++calls < 2;
    });

    uint64_t bytesLeft = 0x1000;
    uint64_t bytesWritten = 0;

    EXPECT_FALSE(WDW::Wipe(nullptr, bytesLeft, bytesWritten, nullptr));
    EXPECT_EQ(bytesLeft, 0x800);
    EXPECT_EQ(bytesWritten, 0x800);
}