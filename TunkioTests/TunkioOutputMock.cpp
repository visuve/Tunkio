#include "PCH.hpp"
#include "TunkioOutputMock.hpp"

namespace Tunkio
{
    uint64_t StreamBufferMock::sputn(const char * data, uint64_t size)
    {
        return data ? size : 0;
    }

    FileStreamMock::FileStreamMock(const FileSystemMock::PathMock& path, int modes) :
        m_path(path),
        m_modes(modes)
    {
    }

    FileStreamMock::operator bool()
    {
        return true;
    }

    uint64_t FileStreamMock::tellp()
    {
        return 1;
    }

    StreamBufferMock* FileStreamMock::rdbuf()
    {
        return &m_buffer;
    }
}