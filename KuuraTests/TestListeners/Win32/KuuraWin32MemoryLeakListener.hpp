#pragma once

namespace Kuura
{
	class Win32MemoryLeakListener : public testing::EmptyTestEventListener
	{
	public:
		Win32MemoryLeakListener();
		~Win32MemoryLeakListener();

		void OnTestStart(const testing::TestInfo&) override;
		void OnTestEnd(const testing::TestInfo& testInfo) override;

	private:
		_CrtMemState _testStart = {};
	};
}