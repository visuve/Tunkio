#pragma once

namespace Kuura
{
	class SpeedListener : public testing::EmptyTestEventListener
	{
	public:
		void OnTestStart(const testing::TestInfo&) override;
		void OnTestEnd(const testing::TestInfo& testInfo) override;

	private:
		std::chrono::high_resolution_clock::time_point _testStart;
	};
}