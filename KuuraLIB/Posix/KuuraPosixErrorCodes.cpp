#include "../KuuraLIB-PCH.hpp"
#include "../KuuraErrorCodes.hpp"

namespace Kuura
{
	namespace ErrorCode
	{
		const int InvalidArgument = EINVAL;
		const int UserCancelled = ECANCELED;
		const int Generic = EPERM;
	}

	std::string SystemErrorCodeToString(uint32_t error)
	{
		const char* errorMsg = strerror(static_cast<int>(error));

		if (!errorMsg)
		{
			return {};
		}

		return { errorMsg };
	}
}
