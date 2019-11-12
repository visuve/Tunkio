#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C"
{
#endif
    typedef enum
    {
        File = 'f',
        Directory = 'd',
        Device = 'D'
    } TunkioTarget;

    typedef enum
    {
        Zeroes = '0',
        Ones = '1',
        LessRandom = 'r',
        MoreRandom = 'R'
    } TunkioMode;

    typedef void(*TunkioProgressCallback)(uint64_t bytesWritten);
    typedef void(*TunkioErrorCallback)(uint32_t error, uint64_t bytesWritten);
    typedef void(*TunkioCompletedCallback)(uint64_t bytesWritten);

    struct TunkioHandle
    {
        int Unused;
    };

    struct TunkioString
    {
        size_t Length;
        char* Data;
    };

    struct TunkioCallbacks
    {
        TunkioProgressCallback ProgressCallback;
        TunkioErrorCallback ErrorCallback;
        TunkioCompletedCallback CompletedCallback;
    };

    struct TunkioOptions
    {
        TunkioTarget Target;
        TunkioMode Mode;
        bool Remove;
        struct TunkioCallbacks Callbacks;
        struct TunkioString Path;
    };

    struct TunkioHandle* __cdecl TunkioCreate(const struct TunkioOptions*);
    uint32_t __cdecl TunkioRun(struct TunkioHandle*);
    void __cdecl TunkioFree(struct TunkioHandle*);

#if defined(__cplusplus)
};
#endif