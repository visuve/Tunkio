#include "../KuuraAPI-PCH.hpp"
#include "KuuraFileFiller.hpp"

namespace Kuura
{
	std::vector<std::byte> ReadContents(const std::filesystem::path& path)
	{
		// TODO: with big files it would be nice to have some kind of progress
		std::basic_ifstream<std::byte> stream(path, std::ios_base::binary);
		return
		{
			std::istreambuf_iterator<std::byte>(stream),
			std::istreambuf_iterator<std::byte>()
		};
	}

	FileFiller::FileFiller(const std::vector<std::byte>& content, bool verify) :
		IFillProvider(verify),
		_fileContent(content)
	{
	}

	FileFiller::FileFiller(const std::filesystem::path& path, bool verify) :
		FileFiller(ReadContents(path), verify)
	{
	}

	bool FileFiller::HasContent()
	{
		return !_fileContent.empty();
	}

	std::span<std::byte> FileFiller::Data(uint64_t bytes, uint64_t alignment)
	{
		assert(HasContent());

		if (_fillData.size() != bytes)
		{
			_fillData.resize(bytes);
			AlignData(alignment);
		}

		for (size_t i = 0; i < bytes; ++i)
		{
			_fillData[i]  = _fileContent[_offset];

			if (++_offset >= _fileContent.size())
			{
				_offset = 0;
			}
		}

		return _fillData;
	}
}
