#pragma once

#if defined(__cplusplus)
extern "C"
{
#endif
    unsigned long  __stdcall TunkioExecuteW(int argc, wchar_t** argv);
    unsigned long  __stdcall TunkioExecuteA(int argc, char** argv);
#if defined(__cplusplus)
};
#endif