#pragma once

#ifdef WIN32
#include <Windows.h>
#else
#include <errno.h>
#endif

namespace Tunkio::ErrorCode
{
	constexpr int Success = 0;

#if defined(_WIN32)
	constexpr int InvalidArgument = ERROR_BAD_ARGUMENTS;
	constexpr int UserCancelled = ERROR_CANCELLED;
	constexpr int Generic = ERROR_INVALID_FUNCTION;
#else
	constexpr int InvalidArgument = EINVAL;
	constexpr int UserCancelled = ECANCELED;
	constexpr int Generic = EPERM;
#endif
}