#include "TunkioTests-PCH.hpp"

namespace Tunkio
{
	int OnStartedCount, OnProgressCount, OnErrorCount, OnCompletedCount = 0;

	void OnWipeStarted(void*, uint16_t, uint64_t)
	{
		++OnStartedCount;
	}

	bool OnProgress(void*, uint16_t, uint64_t)
	{
		++OnProgressCount;
		return true;
	}

	void OnWipeError(void*, TunkioStage, uint16_t, uint64_t, uint32_t)
	{
		++OnErrorCount;
	}

	void OnCompleted(void*, uint16_t, uint64_t)
	{
		++OnCompletedCount;
	}

	void ResetCounters()
	{
		OnStartedCount = 0;
		OnProgressCount = 0;
		OnErrorCount = 0;
		OnCompletedCount = 0;
	}

	TEST(TunkioAPITest, CreateHandleFail)
	{
		TunkioHandle* handle = TunkioInitialize(nullptr, nullptr, TunkioTargetType::File);
		EXPECT_EQ(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 0);
		EXPECT_EQ(OnProgressCount, 0);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 0);
	}

	TEST(TunkioAPITest, CreateHandleSuccess)
	{
		TunkioHandle* handle = TunkioInitialize(nullptr, "foobar", TunkioTargetType::Drive);
		// EXPECT_TRUE(TunkioSetFillMode(handle, TunkioFillType::Random));
		EXPECT_TRUE(TunkioSetRemoveAfterFill(handle, false));
		EXPECT_TRUE(TunkioSetWipeStartedCallback(handle, OnWipeStarted));
		EXPECT_TRUE(TunkioSetProgressCallback(handle, OnProgress));
		EXPECT_TRUE(TunkioSetErrorCallback(handle, OnWipeError));
		EXPECT_TRUE(TunkioSetWipeCompletedCallback(handle, OnCompleted));

		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 0);
		EXPECT_EQ(OnProgressCount, 0);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 0);
	}

	TEST(TunkioAPITest, WipeFileSuccess)
	{
		TunkioHandle* handle = TunkioInitialize(nullptr, "foobar", TunkioTargetType::File);
		//EXPECT_TRUE(TunkioAddRound(handle, TunkioFillType::Random));
		EXPECT_TRUE(TunkioSetRemoveAfterFill(handle, false));
		EXPECT_TRUE(TunkioSetWipeStartedCallback(handle, OnWipeStarted));
		EXPECT_TRUE(TunkioSetProgressCallback(handle, OnProgress));
		EXPECT_TRUE(TunkioSetErrorCallback(handle, OnWipeError));
		EXPECT_TRUE(TunkioSetWipeCompletedCallback(handle, OnCompleted));

		EXPECT_NE(handle, nullptr);
		EXPECT_TRUE(TunkioRun(handle));
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}

	TEST(TunkioAPITest, WipeDirectorySuccess)
	{
		TunkioHandle* handle = TunkioInitialize(nullptr, "foobar", TunkioTargetType::Directory);
		//EXPECT_TRUE(TunkioSetFillMode(handle, TunkioFillType::Random));
		EXPECT_TRUE(TunkioSetRemoveAfterFill(handle, false));
		EXPECT_TRUE(TunkioSetWipeStartedCallback(handle, OnWipeStarted));
		EXPECT_TRUE(TunkioSetProgressCallback(handle, OnProgress));
		EXPECT_TRUE(TunkioSetErrorCallback(handle, OnWipeError));
		EXPECT_TRUE(TunkioSetWipeCompletedCallback(handle, OnCompleted));

		EXPECT_NE(handle, nullptr);
		EXPECT_TRUE(TunkioRun(handle));
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}

	TEST(TunkioAPITest, WipeDeviceSuccess)
	{
		TunkioHandle* handle = TunkioInitialize(nullptr, "foobar", TunkioTargetType::Drive);
		//EXPECT_TRUE(TunkioSetFillMode(handle, TunkioFillType::Random));
		EXPECT_TRUE(TunkioSetRemoveAfterFill(handle, false));
		EXPECT_TRUE(TunkioSetWipeStartedCallback(handle, OnWipeStarted));
		EXPECT_TRUE(TunkioSetProgressCallback(handle, OnProgress));
		EXPECT_TRUE(TunkioSetErrorCallback(handle, OnWipeError));
		EXPECT_TRUE(TunkioSetWipeCompletedCallback(handle, OnCompleted));

		EXPECT_NE(handle, nullptr);
		EXPECT_TRUE(TunkioRun(handle));
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}
}
