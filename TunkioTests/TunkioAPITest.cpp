#include "PCH.hpp"
#include "TunkioAPI.h"
#include "TunkioErrorCodes.hpp"

namespace Tunkio
{
	int OnStartedCount, OnProgressCount, OnErrorCount, OnCompletedCount = 0;

	void OnStarted(uint64_t)
	{
		++OnStartedCount;
	}

	bool OnProgress(uint64_t)
	{
		++OnProgressCount;
		return true;
	}

	void OnError(TunkioStage, uint64_t, uint32_t)
	{
		++OnErrorCount;
	}

	void OnCompleted(uint64_t)
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
		TunkioHandle* handle = TunkioCreate(nullptr);
		EXPECT_EQ(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 0);
		EXPECT_EQ(OnProgressCount, 0);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 0);
	}

	TEST(TunkioAPITest, CreateHandleSuccess)
	{
		const TunkioOptions options
		{
			TunkioTargetType::Device,
			TunkioFillMode::Random,
			false,
			TunkioCallbacks { OnStarted, OnProgress, OnError, OnCompleted },
			TunkioString{ 7, "foobar" }
		};

		TunkioHandle* handle = TunkioCreate(&options);
		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 0);
		EXPECT_EQ(OnProgressCount, 0);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 0);
	}

	TEST(TunkioAPITest, WipeFileSuccess)
	{
		const TunkioOptions options
		{
			TunkioTargetType::File,
			TunkioFillMode::Random,
			false,
			TunkioCallbacks { OnStarted, OnProgress, OnError, OnCompleted },
			TunkioString{ 7, "foobar" }
		};

		TunkioHandle* handle = TunkioCreate(&options);
		EXPECT_EQ(true, TunkioRun(handle));
		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}

	TEST(TunkioAPITest, DirectoryDeviceSuccess)
	{
		const TunkioOptions options
		{
			TunkioTargetType::Directory,
			TunkioFillMode::Random,
			false,
			TunkioCallbacks { OnStarted, OnProgress, OnError, OnCompleted },
			TunkioString{ 7, "foobar" }
		};

		TunkioHandle* handle = TunkioCreate(&options);
		EXPECT_EQ(true, TunkioRun(handle));
		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}

	TEST(TunkioAPITest, WipeDeviceSuccess)
	{
		const TunkioOptions options
		{
			TunkioTargetType::Device,
			TunkioFillMode::Random,
			false,
			TunkioCallbacks { OnStarted, OnProgress, OnError, OnCompleted },
			TunkioString{ 7, "foobar" }
		};

		TunkioHandle* handle = TunkioCreate(&options);
		EXPECT_EQ(true, TunkioRun(handle));
		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(OnStartedCount, 1);
		EXPECT_EQ(OnProgressCount, 1);
		EXPECT_EQ(OnErrorCount, 0);
		EXPECT_EQ(OnCompletedCount, 1);
		ResetCounters();
	}
}