#include "KuuraTests-PCH.hpp"

#if defined(_WIN32) && defined(_DEBUG)
#include "TestListeners/Win32/KuuraWin32MemoryLeakListener.hpp"
#endif

#include "TestListeners/KuuraSpeedListener.hpp"

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
#if defined(_WIN32) && defined(_DEBUG)
	testing::UnitTest::GetInstance()->listeners().Append(new Kuura::Win32MemoryLeakListener());
#endif
	testing::UnitTest::GetInstance()->listeners().Append(new Kuura::SpeedListener());

	return RUN_ALL_TESTS();
}
