#include "PCH.hpp"
#include "Strategies/Win32/TunkioWin32AutoHandle.hpp"

namespace Tunkio::Native::Win32
{
    TEST(AutoHandleTest, IsValid)
    {
        EXPECT_FALSE(Win32AutoHandle(nullptr).IsValid());
        EXPECT_FALSE(Win32AutoHandle(INVALID_HANDLE_VALUE).IsValid());
    }

    TEST(AutoHandleTest, Values)
    {
        {
            void* raw = Win32AutoHandle(nullptr).Handle();
            EXPECT_TRUE(raw == nullptr);
        }
        {
            void* raw = Win32AutoHandle(INVALID_HANDLE_VALUE).Handle();
            EXPECT_TRUE(raw == INVALID_HANDLE_VALUE);
        }
    }
}