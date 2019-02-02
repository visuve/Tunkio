#pragma once

#include <chrono>
#include <iostream>

namespace
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

    std::wostream &operator << (std::wostream& os, const Timer<std::chrono::nanoseconds>& s)
    {
        return os << s.Duration().count() << L"ns";
    }

    std::wostream &operator << (std::wostream& os, const Timer<std::chrono::microseconds>& s)
    {
        return os << s.Duration().count() << L"us";
    }

    std::wostream &operator << (std::wostream& os, const Timer<std::chrono::milliseconds>& s)
    {
        return os << s.Duration().count() << L"ms";
    }

    std::wostream &operator << (std::wostream& os, const Timer<std::chrono::seconds>& s)
    {
        return os << s.Duration().count() << L"s";
    }
}