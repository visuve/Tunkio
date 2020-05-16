#pragma once

#include <chrono>

namespace Tunkio::Time
{
    using Days = std::chrono::duration<uint64_t, std::ratio<86400, 1>>;
    using Hours = std::chrono::duration<uint64_t, std::ratio<3600, 1>>;
    using Minutes = std::chrono::duration<uint64_t, std::ratio<60, 1>>;
    using Seconds = std::chrono::duration<uint64_t, std::ratio<1>>;
    using MilliSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000>>;
    using MicroSeconds = std::chrono::duration<uint64_t, std::ratio<1, 1000'000>>;

    struct Duration
    {
        inline constexpr Duration(const MicroSeconds elapsed) :
            H(std::chrono::duration_cast<Hours>(elapsed)),
            M(std::chrono::duration_cast<Minutes>(elapsed - H)),
            S(std::chrono::duration_cast<Seconds>(elapsed - H - M)),
            Ms(std::chrono::duration_cast<MilliSeconds>(elapsed - H - M - S)),
            Us(elapsed - H - M - S - Ms)
        {
        }

        inline constexpr Duration(const MilliSeconds elapsed) :
            H(std::chrono::duration_cast<Hours>(elapsed)),
            M(std::chrono::duration_cast<Minutes>(elapsed - H)),
            S(std::chrono::duration_cast<Seconds>(elapsed - H - M)),
            Ms(elapsed - H - M - S),
            Us(0)
        {
        }

        inline constexpr Duration(const Seconds elapsed) :
            H(std::chrono::duration_cast<Hours>(elapsed)),
            M(std::chrono::duration_cast<Minutes>(elapsed - H)),
            S(elapsed - H - M),
            Ms(0),
            Us(0)
        {
        }

        inline constexpr Duration(const Minutes elapsed) :
            H(std::chrono::duration_cast<Hours>(elapsed)),
            M(elapsed - H),
            S(0),
            Ms(0),
            Us(0)
        {
        }

        inline constexpr Duration(const Hours elapsed) :
            H(elapsed),
            M(0),
            S(0),
            Ms(0),
            Us(0)
        {
        }

        inline constexpr Duration(const Days elapsed) :
            H(std::chrono::duration_cast<Hours>(elapsed)),
            M(0),
            S(0),
            Ms(0),
            Us(0)
        {
        }

        const Hours H;
        const Minutes M;
        const Seconds S;
        const MilliSeconds Ms;
        const MicroSeconds Us;
    };

    std::ostream& operator << (std::ostream& os, Days s);
    std::ostream& operator << (std::ostream& os, Hours s);
    std::ostream& operator << (std::ostream& os, Minutes s);
    std::ostream& operator << (std::ostream& os, Seconds s);
    std::ostream& operator << (std::ostream& os, MilliSeconds ms);
    std::ostream& operator << (std::ostream& os, MicroSeconds us);
    std::ostream& operator << (std::ostream& os, const Duration& x);

    std::wostream& operator << (std::wostream& os, Days s);
    std::wostream& operator << (std::wostream& os, Hours s);
    std::wostream& operator << (std::wostream& os, Minutes s);
    std::wostream& operator << (std::wostream& os, Seconds s);
    std::wostream& operator << (std::wostream& os, MilliSeconds ms);
    std::wostream& operator << (std::wostream& os, MicroSeconds us);
    std::wostream& operator << (std::wostream& os, const Duration& x);

    std::string HumanReadable(const Duration& duration);

    class Timer
    {
    public:
        template <typename T>
        T Elapsed() const
        {
            const auto diff = std::chrono::high_resolution_clock::now() - m_start;
            return std::chrono::duration_cast<T>(diff);
        }

        void Reset()
        {
            m_start = std::chrono::high_resolution_clock::now();
        }

        Duration Elapsed() const;
    private:
        std::chrono::high_resolution_clock::time_point m_start = std::chrono::high_resolution_clock::now();
    };
}