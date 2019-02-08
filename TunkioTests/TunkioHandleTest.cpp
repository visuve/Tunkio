#include "PCH.hpp"
#include "TunkioHandle.hpp"

namespace Tunkio
{
    TEST(AutoHandleTest, IsValid)
    {
        EXPECT_TRUE(IO::AutoHandle(nullptr).IsValid());
        EXPECT_FALSE(IO::AutoHandle(INVALID_HANDLE_VALUE).IsValid());
    }

    TEST(AutoHandleTest, Operators)
    {
        {
            void* raw = IO::AutoHandle(nullptr);
            EXPECT_TRUE(raw == nullptr);
        }
        {
            void* raw = IO::AutoHandle(INVALID_HANDLE_VALUE);
            EXPECT_TRUE(raw == INVALID_HANDLE_VALUE);
        }
    }
}