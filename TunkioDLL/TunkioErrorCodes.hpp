#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <errno.h>
#endif

namespace Tunkio::ErrorCode
{
    constexpr int Success = 0;

#if defined(_WIN32) || defined(_WIN64)
    constexpr int InvalidArgument = ERROR_BAD_ARGUMENTS;
    constexpr int UserCancelled = ERROR_CANCELLED;
	constexpr int NoData = ERROR_EMPTY;
#else
    constexpr int InvalidArgument = EINVAL;
    constexpr int UserCancelled = ECANCELED;
	constexpr int NoData = ENODATA;
#endif
}