#pragma once

#include <chrono>
// #include <iostream>

namespace Tunkio::Timing
{
    using Days = std::chrono::duration<uint64_t, std::ratio<86400, 1>>;
    using Hours = std::chrono::duration<uint64_t, std::ratio<3600, 1>>;
    using Minutes = std::chrono::duration<uint64_t, std::ratio<60, 1>>;
    using Seconds = std::chrono::duration<uint64_t, std::ratio<1>>;
    using MilliSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
    using MicroSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000'000>>;

    struct Duration
    {
        Duration(const MicroSeconds elapsed);

        const Hours H;
        const Minutes M;
        const Seconds S;
        const MilliSeconds Ms;
        const MicroSeconds Us;
    };

    std::wostream& operator << (std::wostream& os, Days s);
    std::wostream& operator << (std::wostream& os, Hours s);
    std::wostream& operator << (std::wostream& os, Minutes s);
    std::wostream& operator << (std::wostream& os, Seconds s);
    std::wostream& operator << (std::wostream& os, MilliSeconds ms);
    std::wostream& operator << (std::wostream& os, MicroSeconds us);
    std::wostream& operator << (std::wostream& os, const Duration& x);

    class Timer
    {
    public:
        template <typename T>
        T Elapsed() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
        }

        Duration Elapsed() const;
    private:
        const std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
    };
}