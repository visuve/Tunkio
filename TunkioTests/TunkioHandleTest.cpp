#include "PCH.hpp"

#include "TunkioAutoHandle.hpp"

namespace Tunkio
{
	int count;

	void FakeClose(char chr)
	{
		std::cout << "Closing: " << chr << std::endl;
		++count;
	}

	class TestHandle : public AutoHandle<char, 0, 0x7F, FakeClose>
	{
	public:
		 TestHandle(char value) :
			AutoHandle(value)
		{
		}
	};

	TEST(AutoHandleTest, IsValid)
	{
		count = 0;

		EXPECT_FALSE(TestHandle(0).IsValid());
		EXPECT_TRUE(TestHandle('*').IsValid());
		EXPECT_FALSE(TestHandle(127).IsValid());

		EXPECT_EQ(count, 1);
	}

	TEST(AutoHandleTest, Assign)
	{
		count = 0;

		TestHandle handle('a');
		EXPECT_EQ(handle.Value(), 'a');

		handle = 'a'; // TODO: causes unnecessary reopen...
		EXPECT_EQ(count, 1);
		EXPECT_EQ(handle.Value(), 'a');
		
		handle = TestHandle('b');
		EXPECT_EQ(count, 2);
		EXPECT_EQ(handle.Value(), 'b');

		handle = 'c';
		EXPECT_EQ(count, 3);
		EXPECT_EQ(handle.Value(), 'c');
	}
}