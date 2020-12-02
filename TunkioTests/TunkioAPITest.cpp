#include "TunkioTests-PCH.hpp"

namespace Tunkio
{
	struct Counters
	{
		int OnWipeStartedCount = 0;
		int OnPassStartedCount = 0;
		int OnProgressCount = 0;
		int OnPassCompletedCount = 0;
		int OnWipeCompletedCount = 0;
		int OnErrorCount = 0;
	};

	void OnWipeStarted(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnWipeStartedCount++;
	}

	void OnPassStarted(void* context, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassStartedCount++;
	}

	bool OnProgress(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnProgressCount++;
		return true;
	}

	void OnPassCompleted(void* context, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassCompletedCount++;
	}

	void OnWipeCompleted(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnWipeCompletedCount++;
	}

	void OnError(void* context, TunkioStage, uint16_t, uint64_t, uint32_t)
	{
		reinterpret_cast<Counters*>(context)->OnErrorCount++;
	}

	TEST(TunkioAPITest, CreateHandleFail)
	{
		Counters counters;
		TunkioHandle* handle = TunkioInitialize(nullptr, TunkioTargetType::FileWipe, false, &counters);
		EXPECT_EQ(handle, nullptr);

		EXPECT_FALSE(TunkioAddWipeRound(handle, TunkioFillType::ByteFill, false, "xxx"));
		EXPECT_FALSE(TunkioAddWipeRound(handle, TunkioFillType::SequenceFill, false, nullptr));

		TunkioFree(handle);

		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(TunkioAPITest, CreateHandleFailDriveWipe)
	{
		TunkioHandle* handle = TunkioInitialize("foobar", TunkioTargetType::DriveWipe, true, nullptr);
		EXPECT_EQ(handle, nullptr);
	}

	TEST(TunkioAPITest, CreateHandleSuccess)
	{
		Counters counters;
		TunkioHandle* handle = TunkioInitialize("foobar", TunkioTargetType::FileWipe, false, &counters);
		EXPECT_NE(handle, nullptr);
		TunkioFree(handle);

		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(TunkioAPITest, WipeSuccess)
	{
		constexpr TunkioTargetType Types[] =
		{
			TunkioTargetType::FileWipe,
			TunkioTargetType::DirectoryWipe,
			TunkioTargetType::DriveWipe
		};

		for (TunkioTargetType type : Types)
		{
			Counters counters;
			TunkioHandle* handle = TunkioInitialize("foobar", type, false, &counters);
			EXPECT_NE(handle, nullptr);

			EXPECT_EQ(counters.OnWipeStartedCount, 0);
			EXPECT_EQ(counters.OnPassStartedCount, 0);
			EXPECT_EQ(counters.OnProgressCount, 0);
			EXPECT_EQ(counters.OnPassCompletedCount, 0);
			EXPECT_EQ(counters.OnWipeCompletedCount, 0);
			EXPECT_EQ(counters.OnErrorCount, 0);

			TunkioSetWipeStartedCallback(handle, OnWipeStarted);
			TunkioSetPassStartedCallback(handle, OnPassStarted);
			TunkioSetProgressCallback(handle, OnProgress);
			TunkioSetPassCompletedCallback(handle, OnPassCompleted);
			TunkioSetWipeCompletedCallback(handle, OnWipeCompleted);
			TunkioSetErrorCallback(handle, OnError);

			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::OneFill, false, nullptr));
			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::ZeroFill, false, nullptr));
			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::ByteFill, false, "x"));
			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::SequenceFill, false, "xyz"));

			EXPECT_TRUE(TunkioRun(handle));
			TunkioFree(handle);

			EXPECT_EQ(counters.OnWipeStartedCount, 1);
			EXPECT_EQ(counters.OnPassStartedCount, 4);
			EXPECT_EQ(counters.OnProgressCount, 4);
			EXPECT_EQ(counters.OnPassCompletedCount, 4);
			EXPECT_EQ(counters.OnWipeCompletedCount, 1);
			EXPECT_EQ(counters.OnErrorCount, 0);
		}
	}
}
