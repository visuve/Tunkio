#include "TunkioLIB-PCH.hpp"
#include "TunkioTime.hpp"

namespace Tunkio::Time
{
	Duration Duration::None()
	{
		return Duration(Hours(0), Minutes(0), Seconds(0), MilliSeconds(0), MicroSeconds(0));
	}

	Duration Duration::Infinite()
	{
		constexpr auto maximum = std::numeric_limits<uint64_t>::max();

		return Duration(
			Hours(maximum),
			Minutes(maximum),
			Seconds(maximum),
			MilliSeconds(maximum),
			MicroSeconds(maximum));
	}

	Duration::Duration(MicroSeconds elapsed) :
		H(std::chrono::duration_cast<Hours>(elapsed)),
		M(std::chrono::duration_cast<Minutes>(elapsed - H)),
		S(std::chrono::duration_cast<Seconds>(elapsed - H - M)),
		Ms(std::chrono::duration_cast<MilliSeconds>(elapsed - H - M - S)),
		Us(elapsed - H - M - S - Ms)
	{
	}

	Duration::Duration(MilliSeconds elapsed) :
		H(std::chrono::duration_cast<Hours>(elapsed)),
		M(std::chrono::duration_cast<Minutes>(elapsed - H)),
		S(std::chrono::duration_cast<Seconds>(elapsed - H - M)),
		Ms(elapsed - H - M - S),
		Us(0)
	{
	}

	Duration::Duration(Seconds elapsed) :
		H(std::chrono::duration_cast<Hours>(elapsed)),
		M(std::chrono::duration_cast<Minutes>(elapsed - H)),
		S(elapsed - H - M),
		Ms(0),
		Us(0)
	{
	}

	Duration::Duration(Minutes elapsed) :
		H(std::chrono::duration_cast<Hours>(elapsed)),
		M(elapsed - H),
		S(0),
		Ms(0),
		Us(0)
	{
	}

	Duration::Duration(Hours elapsed) :
		H(elapsed),
		M(0),
		S(0),
		Ms(0),
		Us(0)
	{
	}

	Duration::Duration(Days elapsed) :
		H(std::chrono::duration_cast<Hours>(elapsed)),
		M(0),
		S(0),
		Ms(0),
		Us(0)
	{
	}

	bool Duration::operator == (const Duration& other) const
	{
		return Us == other.Us &&
			Ms == other.Ms &&
			S == other.S &&
			M == other.M &&
			H == other.H;
	}

	Duration::Duration(Hours h, Minutes m, Seconds s, MilliSeconds ms, MicroSeconds us) :
		H(h),
		M(m),
		S(s),
		Ms(ms),
		Us(us)
	{
	}

	std::ostream& operator << (std::ostream& os, Days s)
	{
		return os << s.count() << 'd';
	}

	std::ostream& operator << (std::ostream& os, Hours s)
	{
		return os << s.count() << 'h';
	}

	std::ostream& operator << (std::ostream& os, Minutes s)
	{
		return os << s.count() << 'm';
	}

	std::ostream& operator << (std::ostream& os, Seconds s)
	{
		return os << s.count() << 's';
	}

	std::ostream& operator << (std::ostream& os, MilliSeconds ms)
	{
		return os << ms.count() << "ms";
	}

	std::ostream& operator << (std::ostream& os, MicroSeconds us)
	{
		return os << us.count() << "us";
	}

	std::ostream& operator << (std::ostream& os, const Duration& x)
	{
		return os << x.H << ' ' << x.M << ' ' << x.S << ' ' << x.Ms << ' ' << x.Us;
	}

	std::string HumanReadable(const Duration& duration)
	{
		if (duration == Duration::Infinite())
		{
			return "infinite";
		}

		std::stringstream os;

		if (duration.H >= Hours(24))
		{
			os << std::chrono::duration_cast<Days>(duration.H) << ' ';
			os << duration.H % 24 << ' ';
			os << duration.M << ' ';
		}
		else if (duration.H.count() % 24)
		{
			os << duration.H << ' ';
			os << duration.M << ' ';
		}
		else if (duration.M > Minutes(0))
		{
			os << duration.M << ' ';
		}

		os << duration.S;

		return os.str();
	}

	template <typename T>
	std::string Timestamp(const std::chrono::system_clock::time_point& time, T* converterFunction)
	{
		const std::time_t seconds = std::chrono::system_clock::to_time_t(time);
		std::tm dateTime = {};

#if defined(_WIN32)
		if (converterFunction(&dateTime, &seconds) != 0)
#else
		if (converterFunction(&seconds, &dateTime) == nullptr)
#endif
		{
			return {};
		}

		std::array<char, 20> buffer;
		size_t size = std::strftime(buffer.data(), buffer.size(), "%Y-%m-%d %H:%M:%S", &dateTime);
		assert(size == 19);
		return std::string(buffer.data(), size);
	}

	std::string Timestamp(const std::chrono::system_clock::time_point& time)
	{
#if defined(_WIN32)
		return Timestamp(time, localtime_s);
#else
		return Timestamp(time, localtime_r);
#endif
	}

	std::string TimestampUTC(const std::chrono::system_clock::time_point& time)
	{
#if defined(_WIN32)
		return Timestamp(time, gmtime_s);
#else
		return Timestamp(time, gmtime_r);
#endif
	}

	Duration Timer::Elapsed() const
	{
		return Duration(Elapsed<MicroSeconds>());
	}
	}
