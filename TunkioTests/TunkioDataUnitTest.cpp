#include "PCH.hpp"
#include "TunkioDataUnits.hpp"

using namespace std::chrono_literals;

namespace Tunkio
{
    TEST(TunkioDataUnitTest, Bytes)
    {
        EXPECT_EQ(DataUnit::Byte(1).Bytes(), 1);
        EXPECT_EQ(DataUnit::Kibibyte(1).Bytes(), 0x400u);
        EXPECT_EQ(DataUnit::Mebibyte(1).Bytes(), 0x100000u);
        EXPECT_EQ(DataUnit::Gibibyte(1).Bytes(), 0x40000000u);
        EXPECT_EQ(DataUnit::Tebibyte(1).Bytes(), 0x10000000000u);
        EXPECT_EQ(DataUnit::Pebibyte(1).Bytes(), 0x4000000000000u);

        EXPECT_EQ(DataUnit::Byte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Kibibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Mebibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Gibibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Tebibyte(0).Bytes(), 0);
        EXPECT_EQ(DataUnit::Pebibyte(0).Bytes(), 0);
    }

    TEST(TunkioDataUnitTest, CopyConstruct)
    {
        {
            DataUnit::Byte x = DataUnit::Kibibyte(1);
            EXPECT_EQ(x.Bytes(), 1024);
            EXPECT_EQ(x, DataUnit::Kibibyte(1));
        }
        {
            DataUnit::Kibibyte x = DataUnit::Byte(1);
            EXPECT_EQ(x.Bytes(), 1);
            EXPECT_EQ(x, DataUnit::Byte(1));
        }

    }

    TEST(TunkioDataUnitTest, Equals)
    {
        EXPECT_EQ(DataUnit::Byte(1024), DataUnit::Kibibyte(1));
        EXPECT_EQ(DataUnit::Kibibyte(1024), DataUnit::Mebibyte(1));
    }

    TEST(TunkioDataUnitTest, Sum)
    {
        EXPECT_EQ(DataUnit::Mebibyte(3) + DataUnit::Mebibyte(1), DataUnit::Kibibyte(4096));
        EXPECT_EQ(DataUnit::Mebibyte(4) + DataUnit::Kibibyte(1), DataUnit::Kibibyte(4097));
    }

    TEST(TunkioDataUnitTest, Diff)
    {
        EXPECT_EQ(DataUnit::Mebibyte(3) - DataUnit::Mebibyte(1), DataUnit::Kibibyte(2048));
        EXPECT_EQ(DataUnit::Mebibyte(4) - DataUnit::Kibibyte(1), DataUnit::Kibibyte(4095));
    }

    TEST(TunkioDataUnitTest, ToString)
    {
        {
            std::stringstream stream;
            stream <<
                DataUnit::Pebibyte(1) << ' ' <<
                DataUnit::Tebibyte(1) << ' ' <<
                DataUnit::Gibibyte(1) << ' ' <<
                DataUnit::Mebibyte(1) << ' ' <<
                DataUnit::Kibibyte(1) << ' ' <<
                DataUnit::Byte(1);

            EXPECT_STREQ(stream.str().c_str(), "1 pebibyte 1 tebibyte 1 gibibyte 1 mebibyte 1 kibibyte 1 byte");
        }
        {
            std::stringstream stream;
            stream <<
                DataUnit::Pebibyte(2) << ' ' <<
                DataUnit::Tebibyte(2) << ' ' <<
                DataUnit::Gibibyte(2) << ' ' <<
                DataUnit::Mebibyte(2) << ' ' <<
                DataUnit::Kibibyte(2) << ' ' <<
                DataUnit::Byte(2);

            EXPECT_STREQ(stream.str().c_str(), "2 pebibytes 2 tebibytes 2 gibibytes 2 mebibytes 2 kibibytes 2 bytes");
        }
        {
            std::stringstream stream;
            stream << DataUnit::Byte(1024);
            EXPECT_STREQ(stream.str().c_str(), "1024 bytes");
        }
    }

    TEST(TunkioDataUnitTest, HumanReadable)
    {
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Pebibyte(1)).c_str(), "1.000 pebibyte");
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Tebibyte(1024)).c_str(), "1.000 pebibyte");
        EXPECT_STREQ(DataUnit::HumanReadable(DataUnit::Kibibyte(1536)).c_str(), "1.500 mebibytes");
    }

    TEST(TunkioDataUnitTest, Speed)
    {
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(0), Tunkio::Time::MilliSeconds(1000)).c_str(), "unknown");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(1000), Tunkio::Time::MilliSeconds(0)).c_str(), "unknown");

        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(1), Tunkio::Time::MilliSeconds(1000)).c_str(), "1.000 mebibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(2), Tunkio::Time::MilliSeconds(1000)).c_str(), "2.000 mebibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(2), Tunkio::Time::MilliSeconds(4000)).c_str(), "512.000 kibibytes/s");

        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Kibibyte(1536), Tunkio::Time::MilliSeconds(1000)).c_str(), "1.500 mebibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Kibibyte(1536), Tunkio::Time::Seconds(1)).c_str(), "1.500 mebibytes/s");

        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Byte(1536), Tunkio::Time::Seconds(1)).c_str(), "1.500 kibibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Byte(666), Tunkio::Time::Seconds(1)).c_str(), "666.000 bytes/s");


        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Byte(0x40000001), Tunkio::Time::Seconds(1)).c_str(), "1.000 gibibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Byte(0x80000000), Tunkio::Time::Seconds(1)).c_str(), "2.000 gibibytes/s");

        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Mebibyte(2000), Tunkio::Time::Seconds(100)).c_str(), "20.000 mebibytes/s");
        EXPECT_STREQ(DataUnit::SpeedPerSecond(DataUnit::Byte(0x640000000000), Tunkio::Time::Seconds(100)).c_str(), "1.000 tebibytes/s");
    }

    TEST(TunkioDataUnitTest, TimeLeft)
    {
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Byte(1), DataUnit::Byte(1), Tunkio::Time::MilliSeconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 0s);
            EXPECT_EQ(dur.Ms, 1ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Byte(2), DataUnit::Byte(2), Tunkio::Time::MilliSeconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 0s);
            EXPECT_EQ(dur.Ms, 1ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Byte(2), DataUnit::Byte(4), Tunkio::Time::MilliSeconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 0s);
            EXPECT_EQ(dur.Ms, 0ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Byte(4), DataUnit::Byte(2), Tunkio::Time::MilliSeconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 0s);
            EXPECT_EQ(dur.Ms, 2ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(1), DataUnit::Mebibyte(1), Tunkio::Time::Seconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 1s);
            EXPECT_EQ(dur.Ms, 0ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(2), DataUnit::Mebibyte(2), Tunkio::Time::Seconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 1s);
            EXPECT_EQ(dur.Ms, 0ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(2), DataUnit::Mebibyte(4), Tunkio::Time::Seconds(1));
            EXPECT_EQ(dur.H, 0h);
            EXPECT_EQ(dur.M, 0min);
            EXPECT_EQ(dur.S, 0s);
            EXPECT_EQ(dur.Ms, 500ms);
            EXPECT_EQ(dur.Us, 0us);
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(4), DataUnit::Mebibyte(2), Tunkio::Time::Seconds(1));
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
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(0), DataUnit::Mebibyte(100), Tunkio::Time::Seconds(123));
            EXPECT_EQ(dur, Time::Duration::None());
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(100), DataUnit::Mebibyte(0), Tunkio::Time::Seconds(123));
            EXPECT_EQ(dur, Time::Duration::Infinite());
        }
        {
            const Time::Duration dur = DataUnit::TimeLeft(DataUnit::Mebibyte(100), DataUnit::Mebibyte(100), Tunkio::Time::Seconds(0));
            EXPECT_EQ(dur, Time::Duration::None());
        }
    }
}