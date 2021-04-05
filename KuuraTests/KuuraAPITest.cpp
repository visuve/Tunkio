#include "KuuraTests-PCH.hpp"

namespace Kuura
{
	struct Counters
	{
		int OnOverwriteStartedCount = 0;
		int OnPassStartedCount = 0;
		int OnProgressCount = 0;
		int OnPassCompletedCount = 0;
		int OnOverwriteCompletedCount = 0;
		int OnErrorCount = 0;
	};

	void OnOverwriteStarted(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnOverwriteStartedCount++;
	}

	void OnPassStarted(void* context, const KuuraChar*, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassStartedCount++;
	}

	bool OnProgress(void* context, const KuuraChar*, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnProgressCount++;
		return true;
	}

	void OnPassCompleted(void* context, const KuuraChar*, uint16_t)
	{
		reinterpret_cast<Counters*>(context)->OnPassCompletedCount++;
	}

	void OnOverwriteCompleted(void* context, uint16_t, uint64_t)
	{
		reinterpret_cast<Counters*>(context)->OnOverwriteCompletedCount++;
	}

	void OnError(void* context, const KuuraChar*, KuuraStage, uint16_t, uint64_t, uint32_t)
	{
		reinterpret_cast<Counters*>(context)->OnErrorCount++;
	}

	TEST(KuuraAPITest, DecorateFail)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);

		EXPECT_FALSE(KuuraAddTarget(nullptr, KuuraTargetType::FileOverwrite, nullptr, false));
		EXPECT_FALSE(KuuraAddTarget(handle, KuuraTargetType::FileOverwrite, nullptr, false));

		EXPECT_FALSE(KuuraAddPass(handle, KuuraFillType::ByteFill, false, "xxx"));
		EXPECT_FALSE(KuuraAddPass(handle, KuuraFillType::SequenceFill, false, nullptr));

		KuuraFree(handle);

		EXPECT_EQ(counters.OnOverwriteStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnOverwriteCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(KuuraAPITest, RunNoTarget)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);
		EXPECT_TRUE(KuuraAddPass(handle, KuuraFillType::ByteFill, false, "x"));

		EXPECT_FALSE(KuuraRun(handle));
		KuuraFree(handle);

		EXPECT_EQ(counters.OnOverwriteStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnOverwriteCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);
	}

	TEST(KuuraAPITest, CreateHandleSuccess)
	{
		Counters counters;
		KuuraHandle* handle = KuuraInitialize(&counters);
		EXPECT_NE(handle, nullptr);

		EXPECT_TRUE(KuuraAddTarget(handle, KuuraTargetType::FileOverwrite, std::filesystem::path("foobar").c_str(), false));
		EXPECT_EQ(counters.OnOverwriteStartedCount, 0);
		EXPECT_EQ(counters.OnPassStartedCount, 0);
		EXPECT_EQ(counters.OnProgressCount, 0);
		EXPECT_EQ(counters.OnPassCompletedCount, 0);
		EXPECT_EQ(counters.OnOverwriteCompletedCount, 0);
		EXPECT_EQ(counters.OnErrorCount, 0);

		KuuraFree(handle);
	}

	TEST(KuuraAPITest, OverwriteSuccess)
	{
		constexpr KuuraTargetType Types[] =
		{
			KuuraTargetType::FileOverwrite,
			KuuraTargetType::DirectoryOverwrite,
			KuuraTargetType::DriveOverwrite
		};

		for (KuuraTargetType type : Types)
		{
			Counters counters;
			KuuraHandle* handle = KuuraInitialize(&counters);
			EXPECT_TRUE(KuuraAddTarget(handle, type, std::filesystem::path("foobar").c_str(), false));
			EXPECT_NE(handle, nullptr);

			EXPECT_EQ(counters.OnOverwriteStartedCount, 0);
			EXPECT_EQ(counters.OnPassStartedCount, 0);
			EXPECT_EQ(counters.OnProgressCount, 0);
			EXPECT_EQ(counters.OnPassCompletedCount, 0);
			EXPECT_EQ(counters.OnOverwriteCompletedCount, 0);
			EXPECT_EQ(counters.OnErrorCount, 0);

			KuuraSetOverwriteStartedCallback(handle, OnOverwriteStarted);
			KuuraSetPassStartedCallback(handle, OnPassStarted);
			KuuraSetProgressCallback(handle, OnProgress);
			KuuraSetPassCompletedCallback(handle, OnPassCompleted);
			KuuraSetOverwriteCompletedCallback(handle, OnOverwriteCompleted);
			KuuraSetErrorCallback(handle, OnError);

			EXPECT_TRUE(KuuraAddPass(handle, KuuraFillType::OneFill, false, nullptr));
			EXPECT_TRUE(KuuraAddPass(handle, KuuraFillType::ZeroFill, false, nullptr));
			EXPECT_TRUE(KuuraAddPass(handle, KuuraFillType::ByteFill, false, "x"));
			EXPECT_TRUE(KuuraAddPass(handle, KuuraFillType::SequenceFill, false, "xyz"));

			EXPECT_TRUE(KuuraRun(handle));
			KuuraFree(handle);

			EXPECT_EQ(counters.OnOverwriteStartedCount, 1);
			EXPECT_EQ(counters.OnPassStartedCount, 4);
			EXPECT_EQ(counters.OnProgressCount, 8);
			EXPECT_EQ(counters.OnPassCompletedCount, 4);
			EXPECT_EQ(counters.OnOverwriteCompletedCount, 1);
			EXPECT_EQ(counters.OnErrorCount, 0);
		}
	}
}
