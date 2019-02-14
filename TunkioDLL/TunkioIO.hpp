#pragma once

#include "TunkioAPI.h"
#include "TunkioHandle.hpp"
#include "TunkioIOAlias.hpp"

#include <functional>

namespace Tunkio::IO
{
    namespace File
    {
        FileStream Open(const Path& file);
        uint64_t Size(const Path& file);
        bool Fill(FileStream& file, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
        bool Remove(const Path& path);
    }

    namespace Directory
    {
        // TODO!
    }

    namespace Volume
    {
        RawHandle Open(const Path& path);
        uint64_t Size(const AutoHandle& volume);
        bool Fill(const AutoHandle& volume, uint64_t& bytesLeft, uint64_t& writtenBytesTotal, TunkioProgressCallback progress);
    }
}