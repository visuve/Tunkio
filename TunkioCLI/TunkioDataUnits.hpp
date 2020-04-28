#pragma once

#include "TunkioTime.hpp"

namespace Tunkio::DataUnit
{
    template<uint64_t Ratio>
    class DataUnit
    {
    public:
        constexpr DataUnit(size_t value) :
            m_bytes(value * Ratio)
        {
        }

        template<size_t R>
        constexpr DataUnit(const DataUnit<R>& other) :
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

        template<size_t R>
        constexpr bool operator == (const DataUnit<R>& unit) const
        {
            return m_bytes == unit.Bytes();
        }

        template<size_t R>
        DataUnit& operator + (const DataUnit<R>& other)
        {
            m_bytes += other.Bytes();
            return *this;
        }

        template<size_t R>
        DataUnit& operator - (const DataUnit<R>& other)
        {
            m_bytes -= other.Bytes();
            return *this;
        }

    private:
        uint64_t m_bytes;
    };

    using Byte = DataUnit<1>;
    using Kibibyte = DataUnit<0x400>;
    using Mebibyte = DataUnit<0x100000>;
    using Gibibyte = DataUnit<0x40000000>;
    using Tebibyte = DataUnit<0x10000000000>;
    using Pebibyte = DataUnit<0x4000000000000>;


    template<size_t Ratio>
    std::ostream& operator << (std::ostream& os, const DataUnit<Ratio>& unit)
    {
        os << unit.Value() << ' ';

        switch (Ratio)
        {
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

    template<size_t Ratio>
    std::string HumanReadable(const DataUnit<Ratio>& unit)
    {
        std::stringstream os;
        os << std::setprecision(3) << std::fixed;

        if (unit.Bytes() >= 0x4000000000000)
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
}