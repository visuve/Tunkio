#include "../../KuuraTests-PCH.hpp"
#include "KuuraWin32MemoryLeakListener.hpp"

namespace Kuura
{
	Win32MemoryLeakListener::Win32MemoryLeakListener() :
		testing::EmptyTestEventListener()
	{
		for (int mode : { _CRT_WARN, _CRT_ERROR, _CRT_ASSERT })
		{
			_CrtSetReportMode(mode, _CRTDBG_MODE_FILE);
			_CrtSetReportFile(mode, _CRTDBG_FILE_STDERR);
		}
	}

	Win32MemoryLeakListener::~Win32MemoryLeakListener()
	{
	}

	void Win32MemoryLeakListener::OnTestStart(const testing::TestInfo&)
	{
		memset(&_testStart, 0, sizeof(_CrtMemState));

		_CrtMemCheckpoint(&_testStart);
	}

	void Win32MemoryLeakListener::OnTestEnd(const testing::TestInfo& testInfo)
	{
		if (!testInfo.result()->Passed())
		{
			return;
		}

		_CrtMemState testEnd = {};
		_CrtMemState diff = {};
		_CrtMemCheckpoint(&testEnd);

		if (!_CrtMemDifference(&diff, &_testStart, &testEnd))
		{
			return;
		}

		_CrtMemDumpStatistics(&diff);

		GTEST_FAIL() << "\t->\tMemory leak!";
	}
}
