#pragma once

#include <chrono>
#include <iostream>

namespace
{
    template <typename T>
    class StopWatch
    {
    public:
        StopWatch() = default;
        StopWatch(const StopWatch&) = delete;
        StopWatch(StopWatch&&) = delete;
        StopWatch& operator = (const StopWatch&) = delete;
        StopWatch& operator = (StopWatch&&) = delete;
        ~StopWatch() = default;

        T Duration() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
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

    std::wostream &operator << (std::wostream& os, const StopWatch<std::chrono::nanoseconds>& s)
    {
        return os << s.Duration().count() << L"ns";
    }

    std::wostream &operator << (std::wostream& os, const StopWatch<std::chrono::microseconds>& s)
    {
        return os << s.Duration().count() << L"us";
    }

    std::wostream &operator << (std::wostream& os, const StopWatch<std::chrono::milliseconds>& s)
    {
        return os << s.Duration().count() << L"ms";
    }

    std::wostream &operator << (std::wostream& os, const StopWatch<std::chrono::seconds>& s)
    {
        return os << s.Duration().count() << L"s";
    }
}