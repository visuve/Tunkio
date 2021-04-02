#include "KuuraTests-PCH.hpp"

namespace Kuura
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

	void OnPassStarted(void* context, const char*, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassStartedCount++;
	}

	bool OnProgress(void* context, const char*, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnProgressCount++;
		return true;
	}

	void OnPassCompleted(void* context, const char*, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassCompletedCount++;
	}

	void OnWipeCompleted(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnWipeCompletedCount++;
	}

	void OnError(void* context, const char*, KuuraStage, uint16_t, uint64_t, uint32_t)
	{
		reinterpret_cast<Counters*>(context)->OnErrorCount++;
	}

	TEST(KuuraAPITest, DecorateFail)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);

		EXPECT_FALSE(KuuraAddTarget(nullptr, nullptr, KuuraTargetType::FileWipe, false));
		EXPECT_FALSE(KuuraAddTarget(handle, nullptr, KuuraTargetType::FileWipe, false));

		EXPECT_FALSE(KuuraAddWipeRound(handle, KuuraFillType::ByteFill, false, "xxx"));
		EXPECT_FALSE(KuuraAddWipeRound(handle, KuuraFillType::SequenceFill, false, nullptr));

		KuuraFree(handle);

		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(KuuraAPITest, RunNoTarget)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);
		EXPECT_TRUE(KuuraAddWipeRound(handle, KuuraFillType::ByteFill, false, "x"));

		EXPECT_FALSE(KuuraRun(handle));
		KuuraFree(handle);

		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(KuuraAPITest, CreateHandleSuccess)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);

		EXPECT_TRUE(KuuraAddTarget(handle, "foobar", KuuraTargetType::FileWipe, false));
		EXPECT_EQ(counters.OnWipeStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnWipeCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);

		KuuraFree(handle);
	}

	TEST(KuuraAPITest, WipeSuccess)
	{
		constexpr KuuraTargetType Types[] =
		{
			KuuraTargetType::FileWipe,
			KuuraTargetType::DirectoryWipe,
			KuuraTargetType::DriveWipe
		};

		for (KuuraTargetType type : Types)
		{
			Counters counters;
			KuuraHandle* handle = KuuraInitialize(&counters);
			EXPECT_TRUE(KuuraAddTarget(handle, "foobar", type, false));
			EXPECT_NE(handle, nullptr);

			EXPECT_EQ(counters.OnWipeStartedCount, 0);
			EXPECT_EQ(counters.OnPassStartedCount, 0);
			EXPECT_EQ(counters.OnProgressCount, 0);
			EXPECT_EQ(counters.OnPassCompletedCount, 0);
			EXPECT_EQ(counters.OnWipeCompletedCount, 0);
			EXPECT_EQ(counters.OnErrorCount, 0);

			KuuraSetWipeStartedCallback(handle, OnWipeStarted);
			KuuraSetPassStartedCallback(handle, OnPassStarted);
			KuuraSetProgressCallback(handle, OnProgress);
			KuuraSetPassCompletedCallback(handle, OnPassCompleted);
			KuuraSetWipeCompletedCallback(handle, OnWipeCompleted);
			KuuraSetErrorCallback(handle, OnError);

			EXPECT_TRUE(KuuraAddWipeRound(handle, KuuraFillType::OneFill, false, nullptr));
			EXPECT_TRUE(KuuraAddWipeRound(handle, KuuraFillType::ZeroFill, false, nullptr));
			EXPECT_TRUE(KuuraAddWipeRound(handle, KuuraFillType::ByteFill, false, "x"));
			EXPECT_TRUE(KuuraAddWipeRound(handle, KuuraFillType::SequenceFill, false, "xyz"));

			EXPECT_TRUE(KuuraRun(handle));
			KuuraFree(handle);

			EXPECT_EQ(counters.OnWipeStartedCount, 1);
			EXPECT_EQ(counters.OnPassStartedCount, 4);
			EXPECT_EQ(counters.OnProgressCount, 8);
			EXPECT_EQ(counters.OnPassCompletedCount, 4);
			EXPECT_EQ(counters.OnWipeCompletedCount, 1);
			EXPECT_EQ(counters.OnErrorCount, 0);
		}
	}
}
