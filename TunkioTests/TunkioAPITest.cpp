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
		TunkioHandle* handle = TunkioInitialize(&counters, nullptr, TunkioTargetType::File);
		EXPECT_EQ(handle, nullptr);

		EXPECT_FALSE(TunkioAddWipeRound(handle, TunkioFillType::Character, false, "xxx"));
		EXPECT_FALSE(TunkioAddWipeRound(handle, TunkioFillType::Sentence, false, nullptr));

		TunkioFree(handle);

		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(TunkioAPITest, CreateHandleSuccess)
	{
		Counters counters;
		TunkioHandle* handle = TunkioInitialize(&counters, "foobar", TunkioTargetType::File);
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
		for (TunkioTargetType type : {
			TunkioTargetType::File,
			TunkioTargetType::Directory,
			TunkioTargetType::Drive })
		{
			Counters counters;
			TunkioHandle* handle = TunkioInitialize(&counters, "foobar", type);
			EXPECT_NE(handle, nullptr);

			EXPECT_EQ(counters.OnWipeStartedCount, 0);
			EXPECT_EQ(counters.OnPassStartedCount, 0);
			EXPECT_EQ(counters.OnProgressCount, 0);
			EXPECT_EQ(counters.OnPassCompletedCount, 0);
			EXPECT_EQ(counters.OnWipeCompletedCount, 0);
			EXPECT_EQ(counters.OnErrorCount, 0);

			EXPECT_TRUE(TunkioSetWipeStartedCallback(handle, OnWipeStarted));
			EXPECT_TRUE(TunkioSetPassStartedCallback(handle, OnPassStarted));
			EXPECT_TRUE(TunkioSetProgressCallback(handle, OnProgress));
			EXPECT_TRUE(TunkioSetPassCompletedCallback(handle, OnPassCompleted));
			EXPECT_TRUE(TunkioSetWipeCompletedCallback(handle, OnWipeCompleted));
			EXPECT_TRUE(TunkioSetErrorCallback(handle, OnError));

			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::Character, false, "x"));
			EXPECT_TRUE(TunkioAddWipeRound(handle, TunkioFillType::Sentence, false, "xyz"));

			EXPECT_TRUE(TunkioRun(handle));
			TunkioFree(handle);

			EXPECT_EQ(counters.OnWipeStartedCount, 1);
			EXPECT_EQ(counters.OnPassStartedCount, 2);
			EXPECT_EQ(counters.OnProgressCount, 2);
			EXPECT_EQ(counters.OnPassCompletedCount, 2);
			EXPECT_EQ(counters.OnWipeCompletedCount, 1);
			EXPECT_EQ(counters.OnErrorCount, 0);
		}
	}
}
