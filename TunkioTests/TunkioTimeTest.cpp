#include "PCH.hpp"
#include "TunkioTime.hpp"

namespace Tunkio::Time
{
    TEST(TunkioTimeTest, Units)
    {
        {
            Time::Duration duration(Time::MicroSeconds(3600000000));
            EXPECT_EQ(duration.H.count(), 1);
            EXPECT_EQ(duration.M.count(), 0);
            EXPECT_EQ(duration.S.count(), 0);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(3660000000));
            EXPECT_EQ(duration.H.count(), 1);
            EXPECT_EQ(duration.M.count(), 1);
            EXPECT_EQ(duration.S.count(), 0);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(3661000000));
            EXPECT_EQ(duration.H.count(), 1);
            EXPECT_EQ(duration.M.count(), 1);
            EXPECT_EQ(duration.S.count(), 1);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(3661001000));
            EXPECT_EQ(duration.H.count(), 1);
            EXPECT_EQ(duration.M.count(), 1);
            EXPECT_EQ(duration.S.count(), 1);
            EXPECT_EQ(duration.Ms.count(), 1);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(3661001001));
            EXPECT_EQ(duration.H.count(), 1);
            EXPECT_EQ(duration.M.count(), 1);
            EXPECT_EQ(duration.S.count(), 1);
            EXPECT_EQ(duration.Ms.count(), 1);
            EXPECT_EQ(duration.Us.count(), 1);
        }
        {
            Time::Duration duration(Time::MicroSeconds(1));
            EXPECT_EQ(duration.H.count(), 0);
            EXPECT_EQ(duration.M.count(), 0);
            EXPECT_EQ(duration.S.count(), 0);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 1);
        }
        {
            Time::Duration duration(Time::MicroSeconds(1000));
            EXPECT_EQ(duration.H.count(), 0);
            EXPECT_EQ(duration.M.count(), 0);
            EXPECT_EQ(duration.S.count(), 0);
            EXPECT_EQ(duration.Ms.count(), 1);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(1000000));
            EXPECT_EQ(duration.H.count(), 0);
            EXPECT_EQ(duration.M.count(), 0);
            EXPECT_EQ(duration.S.count(), 1);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 0);
        }
        {
            Time::Duration duration(Time::MicroSeconds(60000000));
            EXPECT_EQ(duration.H.count(), 0);
            EXPECT_EQ(duration.M.count(), 1);
            EXPECT_EQ(duration.S.count(), 0);
            EXPECT_EQ(duration.Ms.count(), 0);
            EXPECT_EQ(duration.Us.count(), 0);
        }
    }

    TEST(TunkioTimeTest, ToString)
    {
        {
            std::stringstream stream;
            stream << Time::Days(1) << Time::Hours(1) << Time::Minutes(1) << Time::Seconds(1) << Time::MilliSeconds(1) << Time::MicroSeconds(1);
            EXPECT_STREQ(stream.str().c_str(), "1d1h1m1s1ms1us");
        }
        {
            std::stringstream stream;
            stream << Time::Duration(Time::MicroSeconds(3661001001));
            EXPECT_STREQ(stream.str().c_str(), "1h 1m 1s 1ms 1us");
        }
    }
}