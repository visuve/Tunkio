#include "PCH.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include "TunkioWin32AutoHandle.hpp"

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

	TEST(AutoHandleTest, Reset)
	{
		Win32AutoHandle null(nullptr);
		null.Reset(INVALID_HANDLE_VALUE);
		EXPECT_FALSE(null.IsValid());
	}
}
#endif