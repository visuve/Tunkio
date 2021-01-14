#include "../KuuraLIB-PCH.hpp"
#include "../KuuraErrorCodes.hpp"

namespace Kuura
{
	namespace ErrorCode
	{
		const int InvalidArgument = ERROR_BAD_ARGUMENTS;
		const int UserCancelled = ERROR_CANCELLED;
		const int Generic = ERROR_INVALID_FUNCTION;
	}

	std::string SystemErrorCodeToString(uint32_t error)
	{
		std::array<char, 0x400> buffer = {};

		DWORD size = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			nullptr);

		if (size < 2)
		{
			return {};
		}

		return std::string(buffer.data(), size - 2);
	}
}
