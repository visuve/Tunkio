#include "PCH.hpp"
#include "TunkioNative.hpp"

namespace Tunkio::Native::Win32
{
    TEST(AutoHandleTest, IsValid)
    {
        EXPECT_TRUE(AutoHandle(nullptr).IsValid());
        EXPECT_FALSE(AutoHandle(INVALID_HANDLE_VALUE).IsValid());
    }

    TEST(AutoHandleTest, Operators)
    {
        {
            void* raw = AutoHandle(nullptr);
            EXPECT_TRUE(raw == nullptr);
        }
        {
            void* raw = AutoHandle(INVALID_HANDLE_VALUE);
            EXPECT_TRUE(raw == INVALID_HANDLE_VALUE);
        }
    }
}