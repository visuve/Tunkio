#include "../KuuraAPI-PCH.hpp"
#include "KuuraPosixIO.hpp"

namespace Kuura
{
	std::optional<FileInfo> FileInfoFromDescriptor(int descriptor)
	{
		FileInfo fileInfo = {};

		if (FileInfoFunction(descriptor, &fileInfo) != 0)
		{
			return std::nullopt;
		}

		return fileInfo;
	}
}
