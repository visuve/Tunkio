#pragma once

#include "KuuraTime.hpp"

#include <iomanip>
#include <cmath>

namespace Kuura::DataUnit
{
	template<uint64_t Ratio>
	class DataUnitBase
	{
	public:
		constexpr DataUnitBase() = default;

		constexpr DataUnitBase(uint64_t value) :
			m_bytes(value* Ratio)
		{
		}

		template<uint64_t R>
		constexpr DataUnitBase(const DataUnitBase<R>& other) :
			m_bytes(other.Bytes())
		{
		}

		template<uint64_t R>
		DataUnitBase<Ratio>& operator = (const DataUnitBase<R>& other)
		{
			m_bytes = other.Bytes();
			return *this;
		}

		constexpr uint64_t Bytes() const
		{
			return m_bytes;
		}

		double Value() const
		{
			if (!m_bytes)
			{
				return 0;
			}

			return double(m_bytes) / double(Ratio);
		}

		constexpr bool operator < (uint64_t bytes) const
		{
			return m_bytes < bytes;
		}

		constexpr bool operator > (uint64_t bytes) const
		{
			return m_bytes > bytes;
		}

		template<uint64_t R>
		constexpr bool operator == (const DataUnitBase<R>& unit) const
		{
			return m_bytes == unit.Bytes();
		}

		template<uint64_t R>
		DataUnitBase& operator + (const DataUnitBase<R>& other)
		{
			m_bytes += other.Bytes();
			return *this;
		}

		template<uint64_t R>
		DataUnitBase& operator - (const DataUnitBase<R>& other)
		{
			m_bytes -= other.Bytes();
			return *this;
		}

	private:
		uint64_t m_bytes = 0;
	};

	using Bytes = DataUnitBase<1>;
	using Kibibytes = DataUnitBase<0x400>;
	using Mebibytes = DataUnitBase<0x100000>;
	using Gibibytes = DataUnitBase<0x40000000>;
	using Tebibytes = DataUnitBase<0x10000000000>;
	using Pebibytes = DataUnitBase<0x4000000000000>;
	using Exbibytes = DataUnitBase<0x100000000000000>;

	std::ostream& operator << (std::ostream& os, Bytes bs);
	std::ostream& operator << (std::ostream& os, Kibibytes bs);
	std::ostream& operator << (std::ostream& os, Mebibytes bs);
	std::ostream& operator << (std::ostream& os, Gibibytes bs);
	std::ostream& operator << (std::ostream& os, Tebibytes bs);
	std::ostream& operator << (std::ostream& os, Pebibytes bs);
	std::ostream& operator << (std::ostream& os, Exbibytes bs);

	template<uint64_t Ratio>
	std::string HumanReadable(const DataUnitBase<Ratio>& unit)
	{
		std::stringstream os;
		os << std::setprecision(3) << std::fixed;

		const int exponent = static_cast<int>(std::log(unit.Bytes()) / std::log(1024));

		switch (exponent)
		{
			case 0:
				os << Bytes(unit);
				break;
			case 1:
				os << Kibibytes(unit);
				break;
			case 2:
				os << Mebibytes(unit);
				break;
			case 3:
				os << Gibibytes(unit);
				break;
			case 4:
				os << Tebibytes(unit);
				break;
			case 5:
				os << Pebibytes(unit);
				break;
			case 6:
				os << Exbibytes(unit);
				break;
		}

		assert(exponent >= 0 && exponent <= 6);

		return os.str();
	}

	template<uint64_t Ratio>
	std::string SpeedPerSecond(const DataUnitBase<Ratio>& unit, const Time::MilliSeconds& time)
	{
		const uint64_t millis = time.count();
		const uint64_t bytes = unit.Bytes();

		if (millis == 0 || bytes == 0)
		{
			return "unknown";
		}

		const double elapsedSeconds = double(millis) / 1000.0f;
		const double bytesPerSecond = double(bytes) / elapsedSeconds;
		const int exponent = static_cast<int>(std::log(bytesPerSecond) / std::log(1024));
		const double rounded = bytesPerSecond / std::pow(1024, exponent);

		std::stringstream os;
		os << std::setprecision(3) << std::fixed << rounded;

		switch (exponent)
		{
			case 0:
				os << " bytes/s";
				break;
			case 1:
				os << " kibibytes/s";
				break;
			case 2:
				os << " mebibytes/s";
				break;
			case 3:
				os << " gibibytes/s";
				break;
			case 4:
				os << " tebibytes/s";
				break;
			case 5:
				os << " pebibytes/s";
				break;
			case 6:
				os << " exbibytes/s";
				break;
		}

		assert(exponent >= 0 && exponent <= 6);

		return os.str();
	}

	template<uint64_t Ratio>
	std::string SpeedPerSecond(const DataUnitBase<Ratio>& unit, const Time::Timer& timer)
	{
		return SpeedPerSecond(unit, timer.Elapsed<Time::MilliSeconds>());
	}

	template<uint64_t Ratio>
	Time::Duration TimeLeft(
		const DataUnitBase<Ratio>& bytesLeft,
		const DataUnitBase<Ratio>& bytesWritten,
		const Time::MilliSeconds& elapsed)
	{
		const uint64_t left = bytesLeft.Bytes();

		if (left == 0)
		{
			return Time::Duration::None();
		}

		const uint64_t written = bytesWritten.Bytes();

		if (written == 0)
		{
			return Time::Duration::Infinite();
		}

		const uint64_t millis = elapsed.count();

		if (millis == 0)
		{
			return Time::Duration::None();
		}

		const double bytesPerMilliSecond = double(written) / double(millis);
		const double millisLeft = double(left) / bytesPerMilliSecond;

		return Time::Duration(Kuura::Time::MilliSeconds(uint64_t(millisLeft)));
	}

	template<uint64_t Ratio>
	Time::Duration TimeLeft(
		const DataUnitBase<Ratio>& bytesLeft,
		const DataUnitBase<Ratio>& bytesWritten,
		const Kuura::Time::Timer& elapsed)
	{
		return TimeLeft(bytesLeft, bytesWritten, elapsed.Elapsed<Time::MilliSeconds>());
	}
}
