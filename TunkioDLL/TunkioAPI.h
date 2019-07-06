#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif
    typedef enum
    {
        AutoDetect = 'a',
        File = 'f',
        Directory = 'd',
        Volume = 'v'
    } TunkioTarget;

    typedef enum
    {
        Zeroes = '0',
        Ones = '1',
        LessRandom = 'R',
        MoreRandom = 'r'
    } TunkioMode;

    typedef void(*TunkioProgressCallback)(uint64_t bytesWritten, uint64_t secondsElapsed);

    struct TunkioString
    {
        size_t Length;
        char* Data;
    };

    struct TunkioOptions
    {
        TunkioTarget Target;
        TunkioMode Mode;
        bool Remove;
        TunkioProgressCallback ProgressCallback;
        struct TunkioString Path;
    };

    uint32_t __cdecl TunkioExecute(const struct TunkioOptions* options);

#if defined(__cplusplus)
};
#endif