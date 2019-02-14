#pragma once

//#include <iostream>
//#include <string>

#include "TunkioFileSystemMock.hpp"

namespace Tunkio
{
    class StreamBufferMock
    {
    public:
        StreamBufferMock() = default;
        uint64_t sputn(const char * data, uint64_t size);
    };

    class FileStreamMock
    {
    public:
        FileStreamMock(const FileSystemMock::PathMock& path, int modes);
        operator bool();
        uint64_t tellp();
        StreamBufferMock* rdbuf();

    private:
        const FileSystemMock::PathMock m_path;
        const int m_modes;
        StreamBufferMock m_buffer;
    };
}