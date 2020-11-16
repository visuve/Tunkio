#include "TunkioTests-PCH.hpp"

using namespace std::chrono_literals;

namespace Tunkio
{
	TEST(TunkioDataUnitTest, Bytes)
	{
		EXPECT_EQ(DataUnit::Bytes(1).Bytes(), 1);
		EXPECT_EQ(DataUnit::Kibibytes(1).Bytes(), 0x400u);
		EXPECT_EQ(DataUnit::Mebibytes(1).Bytes(), 0x100000u);
		EXPECT_EQ(DataUnit::Gibibytes(1).Bytes(), 0x40000000u);
		EXPECT_EQ(DataUnit::Tebibytes(1).Bytes(), 0x10000000000u);
		EXPECT_EQ(DataUnit::Pebibytes(1).Bytes(), 0x4000000000000u);

		EXPECT_EQ(DataUnit::Bytes(0).Bytes(), 0);
		EXPECT_EQ(DataUnit::Kibibytes(0).Bytes(), 0);
		EXPECT_EQ(DataUnit::Mebibytes(0).Bytes(), 0);
		EXPECT_EQ(DataUnit::Gibibytes(0).Bytes(), 0);
		EXPECT_EQ(DataUnit::Tebibytes(0).Bytes(), 0);
		EXPECT_EQ(DataUnit::Pebibytes(0).Bytes(), 0);
	}

	TEST(TunkioDataUnitTest, CopyConstruct)
	{
		{
			DataUnit::Bytes x = DataUnit::Kibibytes(1);
			EXPECT_EQ(x.Bytes(), 1024);
			EXPECT_EQ(x, DataUnit::Kibibytes(1));
		}
		{
			DataUnit::Kibibytes x = DataUnit::Bytes(1);
			EXPECT_EQ(x.Bytes(), 1);
			EXPECT_EQ(x, DataUnit::Bytes(1));
		}

	}

	TEST(TunkioDataUnitTest, Equals)
	{
		EXPECT_EQ(DataUnit::Bytes(1024), DataUnit::Kibibytes(1));
		EXPECT_EQ(DataUnit::Kibibytes(1024), DataUnit::Mebibytes(1));
	}

	TEST(TunkioDataUnitTest, Sum)
	{
		EXPECT_EQ(DataUnit::Mebibytes(3) + DataUnit::Mebibytes(1), DataUnit::Kibibytes(4096));
		EXPECT_EQ(DataUnit::Mebibytes(4) + DataUnit::Kibibytes(1), DataUnit::Kibibytes(4097));
	}

	TEST(TunkioDataUnitTest, Diff)
	{
		EXPECT_EQ(DataUnit::Mebibytes(3) - DataUnit::Mebibytes(1), DataUnit::Kibibytes(2048));
		EXPECT_EQ(DataUnit::Mebibytes(4) - DataUnit::Kibibytes(1), DataUnit::Kibibytes(4095));
	}

	TEST(TunkioDataUnitTest, ToString)
	{
		{
			std::stringstream stream;
			stream <<
				DataUnit::Pebibytes(1) << ' ' <<
				DataUnit::Tebibytes(1) << ' ' <<
				DataUnit::Gibibytes(1) << ' ' <<
				DataUnit::Mebibytes(1) << ' ' <<
				DataUnit::Kibibytes(1) << ' ' <<
				DataUnit::Bytes(1);

			EXPECT_STREQ(stream.str().c_str(), "1 pebibyte 1 tebibyte 1 gibibyte 1 mebibyte 1 kibibyte 1 byte");
		}
		{
			std::stringstream stream;
			stream <<
				DataUnit::Pebibytes(2) << ' ' <<
				DataUnit::Tebibytes(2) << ' ' <<
				DataUnit::Gibibytes(2) << ' ' <<
				DataUnit::Mebibytes(2) << ' ' <<
				DataUnit::Kibibytes(2) << ' ' <<
				DataUnit::Bytes(2);

			EXPECT_STREQ(stream.str().c_str(), "2 pebibytes 2 tebibytes 2 gibibytes 2 mebibytes 2 kibibytes 2 bytes");
		}
		{
			std::stringstream stream;
			stream << DataUnit::Bytes(1024);
			EXPECT_STREQ(stream.str().c_str(), "1024 bytes");
		}
	}

	TEST(TunkioDataUnitTest, HumanReadable)
	{
		EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Pebibytes(1)).c_str(), "1.000 pebibyte");
		EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Tebibytes(1024)).c_str(), "1.000 pebibyte");
		EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Kibibytes(1536)).c_str(), "1.500 mebibytes");
	}

	TEST(TunkioDataUnitTest, Speed)
	{
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(0), Tunkio::Time::MilliSeconds(1000)).c_str(), "unknown");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(1000), Tunkio::Time::MilliSeconds(0)).c_str(), "unknown");

		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(1), Tunkio::Time::MilliSeconds(1000)).c_str(), "1.000 mebibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(2), Tunkio::Time::MilliSeconds(1000)).c_str(), "2.000 mebibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(2), Tunkio::Time::MilliSeconds(4000)).c_str(), "512.000 kibibytes/s");

		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Kibibytes(1536), Tunkio::Time::MilliSeconds(1000)).c_str(), "1.500 mebibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Kibibytes(1536), Tunkio::Time::Seconds(1)).c_str(), "1.500 mebibytes/s");

		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Bytes(1536), Tunkio::Time::Seconds(1)).c_str(), "1.500 kibibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Bytes(666), Tunkio::Time::Seconds(1)).c_str(), "666.000 bytes/s");


		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Bytes(0x40000001), Tunkio::Time::Seconds(1)).c_str(), "1.000 gibibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Bytes(0x80000000), Tunkio::Time::Seconds(1)).c_str(), "2.000 gibibytes/s");

		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibytes(2000), Tunkio::Time::Seconds(100)).c_str(), "20.000 mebibytes/s");
		EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Bytes(0x640000000000), Tunkio::Time::Seconds(100)).c_str(), "1.000 tebibytes/s");
	}

	TEST(TunkioDataUnitTest, TimeLeft)
	{
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Bytes(1), DataUnit::Bytes(1), Tunkio::Time::MilliSeconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 0s);
			EXPECT_EQ(dur.Ms, 1ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Bytes(2), DataUnit::Bytes(2), Tunkio::Time::MilliSeconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 0s);
			EXPECT_EQ(dur.Ms, 1ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Bytes(2), DataUnit::Bytes(4), Tunkio::Time::MilliSeconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 0s);
			EXPECT_EQ(dur.Ms, 0ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Bytes(4), DataUnit::Bytes(2), Tunkio::Time::MilliSeconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 0s);
			EXPECT_EQ(dur.Ms, 2ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(1), DataUnit::Mebibytes(1), Tunkio::Time::Seconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 1s);
			EXPECT_EQ(dur.Ms, 0ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(2), DataUnit::Mebibytes(2), Tunkio::Time::Seconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 1s);
			EXPECT_EQ(dur.Ms, 0ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(2), DataUnit::Mebibytes(4), Tunkio::Time::Seconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 0s);
			EXPECT_EQ(dur.Ms, 500ms);
			EXPECT_EQ(dur.Us, 0us);
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(4), DataUnit::Mebibytes(2), Tunkio::Time::Seconds(1));
			EXPECT_EQ(dur.H, 0h);
			EXPECT_EQ(dur.M, 0min);
			EXPECT_EQ(dur.S, 2s);
			EXPECT_EQ(dur.Ms, 0ms);
			EXPECT_EQ(dur.Us, 0us);
		}
	}

	TEST(TunkioDataUnitTest, TimeLeftCornerCases)
	{
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(0), DataUnit::Mebibytes(100), Tunkio::Time::Seconds(123));
			EXPECT_EQ(dur, Time::Duration::None());
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(100), DataUnit::Mebibytes(0), Tunkio::Time::Seconds(123));
			EXPECT_EQ(dur, Time::Duration::Infinite());
		}
		{
			const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibytes(100), DataUnit::Mebibytes(100), Tunkio::Time::Seconds(0));
			EXPECT_EQ(dur, Time::Duration::None());
		}
	}
}
