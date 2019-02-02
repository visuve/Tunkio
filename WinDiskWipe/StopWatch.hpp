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

        virtual ~StopWatch()
        {
            std::cout << *this << std::endl;
        }

        T Duration() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
        }

        StopWatch(const StopWatch&) = delete;
        StopWatch& operator = (const StopWatch&) = delete;

    private:
        const std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
    };

    std::ostream &operator << (std::ostream& os, const StopWatch<std::chrono::nanoseconds>& s)
    {
        return os << s.Duration().count() << "ns";
    }

    std::ostream &operator << (std::ostream& os, const StopWatch<std::chrono::microseconds>& s)
    {
        return os << s.Duration().count() << "us";
    }

    std::ostream &operator << (std::ostream& os, const StopWatch<std::chrono::milliseconds>& s)
    {
        return os << s.Duration().count() << "ms";
    }

    std::ostream &operator << (std::ostream& os, const StopWatch<std::chrono::seconds>& s)
    {
        return os << s.Duration().count() << "s";
    }
}