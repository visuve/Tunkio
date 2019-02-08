#pragma once

#if defined(__cplusplus)
extern "C"
{
#endif
    unsigned long  __stdcall TunkioExecuteW(const wchar_t* path, wchar_t target, wchar_t mode);
    unsigned long  __stdcall TunkioExecuteA(const char* path, char target, char mode);
#if defined(__cplusplus)
};
#endif