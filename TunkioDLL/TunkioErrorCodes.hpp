#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <errno-base.h>
#endif

namespace Tunkio::ErrorCode
{
    constexpr int Success = 0;

#ifdef WIN32
    constexpr int InvalidArgument = ERROR_BAD_ARGUMENTS;
    constexpr int NotImplemented = ERROR_CALL_NOT_IMPLEMENTED;
    constexpr int FileNotFound = ERROR_FILE_NOT_FOUND;
    constexpr int FileEmpty = ERROR_EMPTY;
    constexpr int OpenFailed = ERROR_OPEN_FAILED;
    constexpr int RemoveFailed = ERROR_CANT_DELETE_LAST_ITEM;
#else
    constexpr int InvalidArgument = EINVAL;
    constexpr int NotImplemented = ENOSYS;
    constexpr int FileNotFound = ENOENT;
    constexpr int FileEmpty = ENODATA;
#endif
}