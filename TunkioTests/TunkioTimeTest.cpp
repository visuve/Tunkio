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

	TEST(TunkioTimeTest, HumanReadable)
	{
		{
			{
				Time::Duration duration(Time::Seconds(1));
				EXPECT_STREQ("1s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::Minutes(1));
				EXPECT_STREQ("1m 0s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::Hours(1));
				EXPECT_STREQ("1h 0m 0s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::Hours(24));
				EXPECT_STREQ("1d 0h 0m 0s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::Hours(36));
				EXPECT_STREQ("1d 12h 0m 0s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::Days(2));
				EXPECT_STREQ("2d 0h 0m 0s", Time::HumanReadable(duration).c_str());
			}
			{
				Time::Duration duration(Time::MicroSeconds(3661001001));
				EXPECT_STREQ("1h 1m 1s", Time::HumanReadable(duration).c_str());
			}
			{
				EXPECT_STREQ("infinite", Time::HumanReadable(Time::Duration::Infinite()).c_str());
			}
		}
	}

	TEST(TunkioTimeTest, Timestamp)
	{
		{
			auto localTime = Time::Timestamp();
			EXPECT_FALSE(localTime.empty());

			auto utcTime = Time::TimestampUTC();
			EXPECT_FALSE(utcTime.empty());
		}
		{
			std::time_t tt = 1590229315;
			auto time = std::chrono::system_clock::from_time_t(tt);
			auto timestamp = Time::TimestampUTC(time);
			EXPECT_STREQ("2020-05-23 10:21:55", timestamp.c_str());
		}

	}
}