#include "TunkioTests-PCH.hpp"

#if defined(_WIN32) && defined(_DEBUG)
#include "TestListeners/Win32/TunkioWin32MemoryLeakListener.hpp"
#endif

#include "TestListeners/TunkioSpeedListener.hpp"

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
#if defined(_WIN32) && defined(_DEBUG)
	testing::UnitTest::GetInstance()->listeners().Append(new Tunkio::Win32MemoryLeakListener());
#endif
	testing::UnitTest::GetInstance()->listeners().Append(new Tunkio::SpeedListener());

	return RUN_ALL_TESTS();
}
