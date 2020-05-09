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
        Random = 'r'
    } TunkioMode;

    typedef void(*TunkioStartedCallback)(uint64_t bytesToWrite);
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
        const char* Data;
    };

    struct TunkioCallbacks
    {
        TunkioStartedCallback StartedCallback;
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
    bool __cdecl TunkioRun(struct TunkioHandle*);
    void __cdecl TunkioFree(struct TunkioHandle*);

#if defined(__cplusplus)
};
#endif