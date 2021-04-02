#pragma once

namespace Kuura::Time
{
	using Days = std::chrono::duration<uint64_t, std::ratio<86400, 1>>;
	using Hours = std::chrono::duration<uint64_t, std::ratio<3600, 1>>;
	using Minutes = std::chrono::duration<uint64_t, std::ratio<60, 1>>;
	using Seconds = std::chrono::duration<uint64_t, std::ratio<1>>;
	using MilliSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
	using MicroSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000'000>>;

	class Duration
	{
	public:
		static Duration None();
		static Duration Infinite();

		Duration(MicroSeconds elapsed);
		Duration(MilliSeconds elapsed);
		Duration(Seconds elapsed);
		Duration(Minutes elapsed);
		Duration(Hours elapsed);
		Duration(Days elapsed);
		bool operator == (const Duration& other) const;

		const Hours H;
		const Minutes M;
		const Seconds S;
		const MilliSeconds Ms;
		const MicroSeconds Us;

	private:
		Duration(Hours h, Minutes m, Seconds s, MilliSeconds ms, MicroSeconds us);
	};

	std::ostream& operator << (std::ostream& os, Days s);
	std::ostream& operator << (std::ostream& os, Hours s);
	std::ostream& operator << (std::ostream& os, Minutes s);
	std::ostream& operator << (std::ostream& os, Seconds s);
	std::ostream& operator << (std::ostream& os, MilliSeconds ms);
	std::ostream& operator << (std::ostream& os, MicroSeconds us);
	std::ostream& operator << (std::ostream& os, const Duration& x);

	std::string HumanReadable(const Duration& duration);
	std::string Timestamp(const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());
	std::string TimestampUTC(const std::chrono::system_clock::time_point& time = std::chrono::system_clock::now());

	class Timer
	{
	public:
		template <typename T>
		T Elapsed() const
		{
			const auto diff = std::chrono::high_resolution_clock::now() - _start;
			return std::chrono::duration_cast<T>(diff);
		}

		void Reset()
		{
			_start = std::chrono::high_resolution_clock::now();
		}

		Duration Elapsed() const;
	private:
		std::chrono::high_resolution_clock::time_point _start = std::chrono::high_resolution_clock::now();
	};
}
