#include "../TunkioTests-PCH.hpp"
#include "TunkioSpeedListener.hpp"

namespace Tunkio
{

#if defined(_WIN32) // <- Probably my own machine
	// I would like to use 10ms, but PRNG initialization takes very long
	constexpr std::chrono::milliseconds Limit(50);
#else // <- Probably WSL or CI
	constexpr std::chrono::milliseconds Limit(500);
#endif

	void SpeedListener::OnTestStart(const testing::TestInfo&)
	{
		m_testStart = std::chrono::high_resolution_clock::now();
	}

	void SpeedListener::OnTestEnd(const testing::TestInfo& testInfo)
	{
		if (!testInfo.result()->Passed())
		{
			return;
		}

		const auto elapsed = std::chrono::high_resolution_clock::now() - m_testStart;

		if (elapsed > Limit)
		{
			GTEST_FAIL() << "\t->\tToo slow!";;
		}
	}
}