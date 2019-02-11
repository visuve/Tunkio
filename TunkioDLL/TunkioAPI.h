#pragma once

#include <stdint.h>
#include <wchar.h>

#if defined(__cplusplus)
extern "C"
{
#endif
    typedef void(*TunkioProgressCallback)(uint64_t bytesWritten, uint64_t secondsElapsed);
    unsigned long __stdcall TunkioExecuteW(int argc, wchar_t* argv[], TunkioProgressCallback progress);
    unsigned long __stdcall TunkioExecuteA(int argc, char* argv[], TunkioProgressCallback progress);
#if defined(__cplusplus)
};
#endif