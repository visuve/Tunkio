#include "TunkioTests-PCH.hpp"

#include "FillProviders/TunkioFillProvider.hpp"
#include "FillProviders/TunkioCharFiller.hpp"
#include "FillProviders/TunkioSentenceFiller.hpp"
#include "FillProviders/TunkioRandomFiller.hpp"

namespace Tunkio::Fill
{
	constexpr Tunkio::DataUnit::Kibibytes Kibibyte(1);

	class DummyFiller : public IFillProvider
	{
	public:
		DummyFiller() :
			IFillProvider(Kibibyte, false)
		{
		}

		const void* Data() override
		{
			return nullptr;
		}
	};

	TEST(TunkioFillTest, DummyFill)
	{
		EXPECT_EQ(DummyFiller().Size(1024), 1024);
		EXPECT_EQ(DummyFiller().Size(1023), 1024);
		EXPECT_EQ(DummyFiller().Size(512), 512);
		EXPECT_EQ(DummyFiller().Size(511), 512);
	}

	TEST(TunkioFillTest, ZeroFiller)
	{
		CharFiller filler(Kibibyte, 0xAB, false);

		auto data = reinterpret_cast<const uint8_t*>(filler.Data());

		for (size_t i = 0; i < Kibibyte.Bytes(); ++i)
		{
			EXPECT_EQ(data[i], 0xAB);
		}
	}

	TEST(TunkioFillTest, SentenceFiller)
	{
		SentenceFiller filler(13, "foobar\n", false);

		auto data = reinterpret_cast<const char*>(filler.Data());
		size_t iter = 0;

		for (char c : "foobar\nfoobar\n")
		{
			EXPECT_EQ(c, data[iter++]);
		}
	}

	TEST(TunkioFillTest, RandomFiller)
	{
		RandomFiller filler(Kibibyte, false);
		auto data = filler.Data();
		EXPECT_NE(data, nullptr);
	}

	TEST(TunkioFillTest, RandomFillIllustrateProblem)
	{
		RandomFiller filler(DataUnit::Bytes(10), false);

		auto data = reinterpret_cast<const uint8_t*>(filler.Data());
		EXPECT_NE(data, nullptr);
		EXPECT_EQ(data[8], 0); // Last two remain unrandomized, known issue ;<
		EXPECT_EQ(data[9], 0);
	}
}
