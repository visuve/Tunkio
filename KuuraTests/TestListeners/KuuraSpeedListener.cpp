#include "../KuuraTests-PCH.hpp"
#include "KuuraSpeedListener.hpp"

namespace Kuura
{

#if defined(_WIN32) // <- Probably my own machine
	// I would like to use 10ms, but PRNG initialization takes very long
	constexpr std::chrono::milliseconds Limit(50);
#else // <- Probably WSL or CI
	constexpr std::chrono::milliseconds Limit(500);
#endif

	void SpeedListener::OnTestStart(const testing::TestInfo&)
	{
		_testStart = std::chrono::high_resolution_clock::now();
	}

	void SpeedListener::OnTestEnd(const testing::TestInfo& testInfo)
	{
		if (!testInfo.result()->Passed())
		{
			return;
		}

#if defined(_WIN32)
		if (IsDebuggerPresent())
		{
			return;
		}
#endif

		const auto elapsed = std::chrono::high_resolution_clock::now() - _testStart;

		if (elapsed > Limit)
		{
			GTEST_FAIL() << "\t->\tToo slow!";;
		}
	}
}
