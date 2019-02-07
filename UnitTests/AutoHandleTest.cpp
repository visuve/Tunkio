#include "PCH.hpp"
#include "../WinDiskWipe/AutoHandle.hpp"

TEST(AutoHandleTest, IsValid) 
{
    EXPECT_TRUE(WDW::AutoHandle(nullptr).IsValid());
    EXPECT_FALSE(WDW::AutoHandle(INVALID_HANDLE_VALUE).IsValid());
}

TEST(AutoHandleTest, Operators)
{
    {
        void* raw = WDW::AutoHandle(nullptr);
        EXPECT_TRUE(raw == nullptr);
    }
    {
        void* raw = WDW::AutoHandle(INVALID_HANDLE_VALUE);
        EXPECT_TRUE(raw == INVALID_HANDLE_VALUE);
    }
}