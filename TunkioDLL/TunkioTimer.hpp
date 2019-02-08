#pragma once

#include "TunkioUnits.hpp"

#include <chrono>
#include <iostream>

namespace Tunkio
{
    template <typename T>
    class Timer
    {
    public:
        Timer() = default;
        Timer(const Timer&) = delete;
        Timer(Timer&&) = delete;
        Timer& operator = (const Timer&) = delete;
        Timer& operator = (Timer&&) = delete;
        ~Timer() = default;

        T Duration() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
        }

        uint64_t Count() const
        {
            return Duration().count();
        }

        bool operator < (T time) const
        {
            return Duration() < time;
        }

        bool operator > (T time) const
        {
            return Duration() > time;
        }

    private:
        const std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
    };

    inline std::wostream& operator << (std::wostream& os, const Timer<Units::MicroSeconds>& us)
    {
        return os << us.Count() << L"us";
    }

    inline std::wostream& operator << (std::wostream& os, const Timer<Units::MilliSeconds>& ms)
    {
        return os << ms.Count() << L"ms";
    }

    inline std::wostream& operator << (std::wostream& os, const Timer<Units::Seconds>& s)
    {
        return os << s.Count() << L"s";
    }

    inline std::wostream& operator << (std::wostream& os, const Timer<Units::Minutes>& m)
    {
        return os << m.Count() << L"m";
    }

    inline std::wostream& operator << (std::wostream& os, const Timer<Units::Hours>& h)
    {
        return os << h.Count() << L"h";
    }
}