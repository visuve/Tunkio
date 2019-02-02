#pragma once

#include <chrono>

namespace Units
{
    using MicroSeconds = std::chrono::duration<uint64_t, std::micro>;
    using MilliSeconds = std::chrono::duration<uint64_t, std::milli>;
    using Seconds = std::chrono::duration<uint64_t>;
    using Minutes = std::chrono::duration<uint64_t, std::ratio<60>>;
    using Hours = std::chrono::duration<uint64_t, std::ratio<3600>>;

    constexpr uint32_t KiloByte = 1024;
    constexpr uint32_t MegaByte = KiloByte * 1024;
    constexpr uint32_t GigaByte = MegaByte * 1024;
}