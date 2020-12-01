#pragma once

namespace Tunkio
{
	namespace ErrorCode
	{
		constexpr int Success = 0;
#if defined(_WIN32)
		extern const int InvalidArgument;
		extern const int UserCancelled;
		extern const int Generic;
#else
		extern const int InvalidArgument;
		extern const int UserCancelled;
		extern const int Generic;
#endif
	}

	std::string SystemErrorCodeToString(uint32_t);
}
