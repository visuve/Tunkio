#pragma once

#include "TunkioTime.hpp"

#undef min
#undef max

namespace Tunkio::DataUnit
{
    template<uint64_t Ratio>
    class DataUnit
    {
    public:
        constexpr DataUnit() = default;

        constexpr DataUnit(uint64_t value) :
            m_bytes(value * Ratio)
        {
        }

        template<uint64_t R>
        constexpr DataUnit(const DataUnit<R>& other) :
            m_bytes(other.Bytes())
        {
        }

        template<uint64_t R>
        DataUnit<Ratio>& operator = (const DataUnit<R>& other) :
            m_bytes(other.Bytes())
        {
        }

        uint64_t Bytes() const
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

        template<uint64_t R>
        constexpr bool operator == (const DataUnit<R>& unit) const
        {
            return m_bytes == unit.Bytes();
        }

        template<uint64_t R>
        DataUnit& operator + (const DataUnit<R>& other)
        {
            m_bytes += other.Bytes();
            return *this;
        }

        template<uint64_t R>
        DataUnit& operator - (const DataUnit<R>& other)
        {
            m_bytes -= other.Bytes();
            return *this;
        }

    private:
        uint64_t m_bytes = 0;
    };

    using Byte = DataUnit<1>;
    using Kibibyte = DataUnit<0x400>;
    using Mebibyte = DataUnit<0x100000>;
    using Gibibyte = DataUnit<0x40000000>;
    using Tebibyte = DataUnit<0x10000000000>;
    using Pebibyte = DataUnit<0x4000000000000>;
    using Exbibyte = DataUnit<0x100000000000000>;

    template<uint64_t Ratio>
    std::ostream& operator << (std::ostream& os, const DataUnit<Ratio>& unit)
    {
        os << unit.Value() << ' ';

        switch (Ratio)
        {
            case 0x100000000000000:
                os << "exbibyte";
                break;
            case 0x4000000000000:
                os << "pebibyte";
                break;
            case 0x10000000000:
                os << "tebibyte";
                break;
            case 0x40000000:
                os << "gibibyte";
                break;
            case 0x100000:
                os << "mebibyte";
                break;
            case 0x400:
                os << "kibibyte";
                break;
            default:
                os << "byte";
                break;
        }

        if (unit.Value() > 1)
        {
            os << 's';
        }

        return os;
    }

    template<uint64_t Ratio>
    std::string HumanReadable(const DataUnit<Ratio>& unit)
    {
        std::stringstream os;
        os << std::setprecision(3) << std::fixed;
        
        if (unit.Bytes() >= 0x100000000000000)
        {
            os << Pebibyte(unit);
        }
        else if (unit.Bytes() >= 0x4000000000000)
        {
            os << Pebibyte(unit);
        }
        else if (unit.Bytes() >= 0x10000000000)
        {
            os << Tebibyte(unit);
        }
        else if (unit.Bytes() >= 0x40000000)
        {
            os << Gibibyte(unit);
        }
        else if (unit.Bytes() >= 0x100000)
        {
            os << Mebibyte(unit);
        }
        else if (unit.Bytes() >= 0x400)
        {
            os << Kibibyte(unit);
        }
        else
        {
            os << Byte(unit);
        }

        return os.str();
    }

    template<uint64_t T>
    std::string SpeedPerSecond(const DataUnit<T>& unit, const Tunkio::Time::MilliSeconds& time)
    {
        const uint64_t millis = time.count();
        const uint64_t bytes = unit.Bytes();

        if (millis == 0 || bytes == 0)
        {
            return "unknown";
        }

        const double elapsedSeconds = millis / 1000.0f;
        const double bytesPerSecond = bytes / elapsedSeconds;

        std::stringstream os;
        os << std::setprecision(3) << std::fixed;

        if (bytesPerSecond >= 0x100000000000000)
        {
            os << bytesPerSecond / 0x100000000000000 << " exbibytes/s";
        }
        else if (bytesPerSecond >= 0x4000000000000)
        {
            os << bytesPerSecond / 0x4000000000000 << " pebibytes/s";
        }
        else if (bytesPerSecond >= 0x10000000000)
        {
            os << bytesPerSecond / 0x10000000000 << " tebibytes/s";
        }
        else if (bytesPerSecond >= 0x40000000)
        {
            os << bytesPerSecond / 0x40000000 << " gibibytes/s";
        }
        else if (bytesPerSecond >= 0x100000)
        {
            os << bytesPerSecond / 0x100000 << " mebibytes/s";
        }
        else if (bytesPerSecond >= 0x400)
        {
            os << bytesPerSecond / 0x400 << " kibibytes/s";
        }
        else
        {
            os << bytesPerSecond << " bytes/s";
        }

        return os.str();
    }

    template<uint64_t T>
    std::string SpeedPerSecond(const DataUnit<T>& unit, const Tunkio::Time::Timer& timer)
    {
        return SpeedPerSecond(unit, timer.Elapsed<Time::MilliSeconds>());
    }

    template<uint64_t T>
    Time::Duration TimeLeft(const DataUnit<T>& bytesLeft, const DataUnit<T>& bytesWritten, const Tunkio::Time::MilliSeconds& time)
    {
        const uint64_t millis = time.count();
        const uint64_t bytesW = bytesWritten.Bytes();
        const uint64_t bytesL = bytesLeft.Bytes();

        if (millis == 0 || bytesW == 0 || bytesL == 0)
        {
            //return Time::Duration(0);
            throw "FUBAR";
        }

        const double bytesPerMilliSecond = double(bytesW) / double(millis);
        const double millisLeft = double(bytesL) / bytesPerMilliSecond;

        return Time::Duration(Tunkio::Time::MilliSeconds(uint64_t(millisLeft)));
    }

    template<uint64_t T>
    Time::Duration TimeLeft(const DataUnit<T>& bytesLeft, const DataUnit<T>& bytesWritten, const Tunkio::Time::Timer& timer)
    {
        return TimeLeft(bytesLeft, bytesWritten, timer.Elapsed<Time::MilliSeconds>());
    }
}